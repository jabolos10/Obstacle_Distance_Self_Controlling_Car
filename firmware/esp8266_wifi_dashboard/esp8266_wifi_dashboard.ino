#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

float current_distance = 0;
float target_distance = 25.0;
uint32_t update_count = 0;

bool target_changed = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("DistanceMonitor", "12345678");

  server.on("/", []() {
    String html = "<!DOCTYPE html><html><head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Distance Monitor</title>";
    html += "<style>";
    html += "body { font-family: Arial; text-align: center; margin-top: 50px; background: #f0f0f0; }";
    html += ".container { background: white; border-radius: 20px; padding: 30px; margin: 20px auto; max-width: 600px; box-shadow: 0 0 20px rgba(0,0,0,0.1); }";
    html += ".value { font-size: 80px; font-weight: bold; margin: 20px 0; color: #333; }";
    html += ".label { font-size: 24px; color: #666; }";
    html += "input { font-size: 30px; padding: 15px; width: 150px; text-align: center; border: 2px solid #ddd; border-radius: 10px; }";
    html += "button { font-size: 30px; padding: 15px 30px; background: #4CAF50; color: white; border: none; border-radius: 10px; cursor: pointer; margin-left: 10px; }";
    html += "button:hover { background: #45a049; }";
    html += ".status-box { margin: 20px auto; padding: 15px; border-radius: 10px; font-size: 24px; font-weight: bold; width: 80%; }";
    html += ".forward { background: #4CAF50; color: white; }";
    html += ".backward { background: #f44336; color: white; }";
    html += ".idle { background: #ff9800; color: white; }";
    html += "</style>";

    html += "<script>";
    html += "function loadData() {";
    html += "  fetch('/data')";
    html += "    .then(response => response.json())";
    html += "    .then(data => {";
    html += "      document.getElementById('current').innerHTML = data.distance.toFixed(1);";
    html += "      document.getElementById('target_display').innerHTML = data.target.toFixed(1);";
    html += "      document.getElementById('count').innerHTML = data.count;";

    html += "      var statusDiv = document.getElementById('status');";
    html += "      if (data.distance > data.target + 1) {";
    html += "        statusDiv.className = 'status-box forward';";
    html += "        statusDiv.innerHTML = '\xF0\x9F\x9A\x97 GOING FORWARD';";
    html += "      } else if (data.distance < data.target - 1) {";
    html += "        statusDiv.className = 'status-box backward';";
    html += "        statusDiv.innerHTML = '\xE2\xAC\x85 GOING BACKWARD';";
    html += "      } else {";
    html += "        statusDiv.className = 'status-box idle';";
    html += "        statusDiv.innerHTML = '\xE2\x8F\xB1 TARGET REACHED';";
    html += "      }";
    html += "    });";
    html += "}";
    html += "setInterval(loadData, 200);";

    html += "function sendTarget() {";
    html += "  var val = document.getElementById('target_input').value;";
    html += "  fetch('/set?val=' + val)";
    html += "    .then(() => {";
    html += "      document.getElementById('target_display').innerHTML = parseFloat(val).toFixed(1);";
    html += "    });";
    html += "}";
    html += "</script>";

    html += "</head><body>";
    html += "<div class='container'>";
    html += "<h1>\xF0\x9F\x93\x8F DISTANCE MONITOR</h1>";

    html += "<div id='status' class='status-box idle'>\xE2\x8F\xB1 TARGET REACHED</div>";

    html += "<div class='label'>AKTUALNA ODLEG\xC5\x81O\xC5\x9A\xC4\x86</div>";
    html += "<div class='value' id='current'>" + String(current_distance) + "</div>";

    html += "<div class='label'>WARTO\xC5\x9A\xC4\x86 ZADANA</div>";
    html += "<div class='value' id='target_display'>" + String(target_distance) + "</div>";

    html += "<div style='margin: 40px 0;'>";
    html += "<input type='number' id='target_input' step='0.1' value='" + String(target_distance) + "'>";
    html += "<button onclick='sendTarget()'>USTAW</button>";
    html += "</div>";

    html += "<p>Aktualizacji: <span id='count'>" + String(update_count) + "</span></p>";
    html += "<p>IP: " + WiFi.softAPIP().toString() + "</p>";
    html += "</div>";
    html += "</body></html>";

    server.send(200, "text/html", html);
  });

  server.on("/set", []() {
    if (server.hasArg("val")) {
      target_distance = server.arg("val").toFloat();
      target_changed = true;
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/data", []() {
    String json = "{";
    json += "\"distance\":" + String(current_distance) + ",";
    json += "\"target\":" + String(target_distance) + ",";
    json += "\"count\":" + String(update_count);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  server.handleClient();

  // Receiving the distance from STM32
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();

    if (data.length() > 0) {
      float val = data.toFloat();
      if (val > 0 || data == "0.0") {
        current_distance = val;
        update_count++;
      }
    }
  }

  // Sending the target distance to STM32
  if (target_changed) {
    // "%f" needs ~8+ bytes (e.g. "25.000000\0") — widen this buffer accordingly.
    char buf[3];
    sprintf(buf, "%f", target_distance);
    Serial.write(buf);

    target_changed = false;
  }

  delay(10);
}
