#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_
#include "main.h"

// Values not given in the report — placeholders, pending confirmation
// against the real TIM4 auto-reload/CCR config once the full project lands.
#define PWM_MAX 1000
#define MIN_PWM 200

extern TIM_HandleTypeDef htim4;

void SetRobotSpeed(int32_t speed);

#endif /* INC_MOTOR_CONTROL_H_ */
