#ifndef INC_PD_CONTROLLER_H_
#define INC_PD_CONTROLLER_H_
#include "main.h"

#define TOLERANCE_CM 1.0f

typedef struct
{
    float Kp;
    float Kd;
    float target_distance;
    float prev_error;
    uint32_t last_time;
} PD_Controller;

float Calculate_PD(PD_Controller *pd, uint32_t *current_distance);

#endif /* INC_PD_CONTROLLER_H_ */
