/*
 * These are the main.c fragments visible in the lab report screenshots —
 * NOT a complete, compilable file. Line numbers in the comments match the
 * report's code editor screenshots, to make it easier to slot them back
 * into the real main.c once the full firmware project is available (see
 * firmware/stm32/README.md).
 */

#include "main.h"
#include "hc_sr_04.h"
#include "pd_controller.h"
#include "motor_control.h"

extern struct us_sensor_str distance_sensor;
extern PD_Controller robotPD;
extern UART_HandleTypeDef huart2; // to the ESP8266 WiFi module
extern UART_HandleTypeDef huart3; // to the second UART (e.g. HC-05 / debug)
extern uint8_t rx_data[8];

static uint32_t last_pd_calc_time;
static int32_t current_pwm_output;

/* ---- lines ~110-123: ASCII byte-triplet -> float parser ---- */
float uint8_to_float_ascii(uint8_t bytes[3])
{
    // Assuming bytes contain ASCII digits: '3', '0', '0'
    if (bytes[0] != '.' && bytes[1] != '.' && bytes[2] != '.') {
        int value = (bytes[0] - '0') * 100 +
                    (bytes[1] - '0') * 10 +
                    (bytes[2] - '0');
        return (float)value;
    }
    else if (bytes[2] == '.') {
        int value = (bytes[0] - '0') * 10 + (bytes[1] - '0');
        return (float)value;
    }
    else if (bytes[1] == '.') {
        int value = (bytes[0] - '0');
        return (float)value;
    }
    return 0.0f; // NOTE: no explicit fallback shown in the report excerpt
}

/* ---- lines ~255-265: 50ms PD control cycle, called from the main loop ---- */
void control_loop_step(void)
{
    if (HAL_GetTick() - last_pd_calc_time > 50)
    {
        last_pd_calc_time = HAL_GetTick();

        // 1. Oblicz nowe PWM na podstawie zmiennej z czujnika
        float calculated_pwm = Calculate_PD(&robotPD, &distance_sensor.distance_cm);

        // 2. Przekaz do silnikow
        current_pwm_output = (int32_t)calculated_pwm;
        SetRobotSpeed(current_pwm_output);
    }
}

/* ---- lines ~268-274: forwarding the live distance to the WiFi module ---- */
void send_distance_to_wifi_module(void)
{
    float distances = (float)distance_sensor.distance_cm;

    char wifi_buffer[50];
    sprintf(wifi_buffer, "%.1f\n", distances);
    HAL_UART_Transmit(&huart2, (uint8_t *)wifi_buffer, strlen(wifi_buffer), 1000);
}

/* ---- lines 334-344: HC-SR04 echo capture (input-capture interrupt) ---- */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (TIM1 == htim->Instance)
    {
        uint32_t echo_us;

        echo_us = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        distance_sensor.distance_cm = hc_sr04_convert_us_to_cm(echo_us);
    }
}

/* ---- lines 345-355: UART Rx complete (echoes received bytes back out) ---- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_UART_RxCpltCallback can be implemented in the user file.
    */
    HAL_UART_Transmit(&huart3, rx_data, 8, 10);
}
