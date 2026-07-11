# STM32 firmware notes

Driver and control-logic reference for the ultrasonic sensor, PD controller, motor driver, and LCD.

## Layout

| File | Purpose |
|---|---|
| [`Drivers/hc_sr_04.c`](Drivers/hc_sr_04.c) / `.h` | Ultrasonic sensor driver — timer PWM trigger + input-capture echo timing |
| [`Drivers/lcd_i2c.c`](Drivers/lcd_i2c.c) / `.h` | HD44780 LCD driver over I2C |
| [`Core/pd_controller.c`](Core/pd_controller.c) / `.h` | PD distance controller |
| [`Core/motor_control.c`](Core/motor_control.c) / `.h` | DRV8833 PWM motor driver |
| [`Core/main_excerpts.c`](Core/main_excerpts.c) | ISR callbacks (timer capture, UART Rx) and the control-loop glue that ties the above together |

## Configuration

`SLAVE_ADDRESS_LCD` and the HD44780 command bytes in [`lcd_i2c.h`](Drivers/lcd_i2c.h) use standard PCF8574/HD44780 defaults. `PWM_MAX` / `MIN_PWM` in [`motor_control.h`](Core/motor_control.h) are tuning parameters — adjust to match your DRV8833 wiring, battery voltage, and desired top speed.

## Notes

The ESP8266 dashboard sketch ([`../esp8266_wifi_dashboard/`](../esp8266_wifi_dashboard)) formats the target distance with `sprintf(buf, "%f", target_distance)` into a 3-byte buffer, which is undersized for `%f` output — widen it to at least 12 bytes.

## License

[MIT](../../LICENSE)
