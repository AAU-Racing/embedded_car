#ifndef GEAR_H
#define GEAR_H

#include <stm32f4xx_hal.h>
#include <stdbool.h>

HAL_StatusTypeDef init_gear();
uint8_t gear_number();
bool change_gear();

#endif
