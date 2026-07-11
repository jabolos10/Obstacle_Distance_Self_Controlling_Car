# STM32 firmware — status

This folder is a **reconstruction from the lab report** (`docs/report/`), not a copy of the original STM32CubeIDE project. The teammate's firmware repository referenced in the report (`YaqbK/autko`) is no longer reachable, and the full `.ioc`/`Core`/`Drivers` project hasn't been shared with this repo yet.

## What's verbatim from the report

- [`Drivers/hc_sr_04.h`](Drivers/hc_sr_04.h) / [`hc_sr_04.c`](Drivers/hc_sr_04.c) — full ultrasonic driver.
- [`Core/pd_controller.h`](Core/pd_controller.h) / [`pd_controller.c`](Core/pd_controller.c) — full `Calculate_PD` implementation (struct fields inferred from usage).
- [`Core/motor_control.h`](Core/motor_control.h) / [`motor_control.c`](Core/motor_control.c) — full `SetRobotSpeed` implementation.
- [`Core/main_excerpts.c`](Core/main_excerpts.c) — verbatim fragments: `uint8_to_float_ascii`, the 50 ms PD control cycle, the distance-to-WiFi UART send, `HAL_TIM_IC_CaptureCallback`, `HAL_UART_RxCpltCallback`.
- [`Drivers/lcd_i2c.c`](Drivers/lcd_i2c.c) — `lcd_send_cmd`, `lcd_send_data`, `lcd_init`, `distance_display` are verbatim; `lcd_clear`, `lcd_put_cursor`, `lcd_send_string` are referenced but their bodies were never shown in the report, so only declarations exist in the header.

## Known gaps / placeholders

- `PWM_MAX`, `MIN_PWM` ([`motor_control.h`](Core/motor_control.h)) and the LCD I2C address / HD44780 command bytes ([`lcd_i2c.h`](Drivers/lcd_i2c.h)) are typical/standard values, **not confirmed** against the real hardware config — flagged inline.
- No `.ioc` file, linker script, or HAL config (`main.c` init, `MX_TIM4_Init`, `MX_I2C1_Init`, etc.) — those weren't captured in the report.
- `PD_Controller robotPD`, `distance_sensor`, and the UART handles in `main_excerpts.c` are declared `extern` — their real initialization lives in the missing `main.c`.

## Known issue carried over from the report

In [`firmware/esp8266_wifi_dashboard/esp8266_wifi_dashboard.ino`](../esp8266_wifi_dashboard/esp8266_wifi_dashboard.ino), `loop()` does:

```cpp
char buf[3];
sprintf(buf, "%f", target_distance);
```

`"%f"` needs at least ~8 bytes (e.g. `"25.000000"` plus the null terminator); `buf[3]` overflows on every call. This is transcribed as-is from the report screenshot — worth fixing once the real firmware is merged in.

## Next step

Once the actual STM32CubeIDE project is available, replace this folder with the real `Core/` and `Drivers/` trees and delete this note.
