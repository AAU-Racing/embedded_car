#include <stm32f4xx_hal.h>

#include <stdbool.h>

bool get_wheel_speed_FL(float* speed);
bool get_wheel_speed_FR(float* speed);
bool get_wheel_speed_RL(float* speed);
bool get_wheel_speed_RR(float* speed);
void wheel_speed_callback(CanRxMsgTypeDef* Msg);
HAL_StatusTypeDef wheel_speed_init(void);
