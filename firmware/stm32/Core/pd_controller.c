#include "pd_controller.h"

float Calculate_PD(PD_Controller *pd, uint32_t *current_distance)
{
    uint32_t now = HAL_GetTick();
    float dt = (now - pd->last_time) / 1000.0f;
    if (dt <= 0.0f) dt = 0.001f;

    float current_distance_f = *current_distance;
    float error = current_distance_f - pd->target_distance;

    if (error > -TOLERANCE_CM && error < TOLERANCE_CM) {
        pd->prev_error = error;
        pd->last_time = now;

        return 0.0f;
    }

    float derivative = (error - pd->prev_error) / dt;

    float output = (pd->Kp * error) + (pd->Kd * derivative);

    pd->prev_error = error;
    pd->last_time = now;

    return output;
}
