#ifndef GEAR_H
#define GEAR_H

#include <stm32f4xx_hal.h>
#include <stdbool.h>
#include "hbridge.h"

#define gear_forward()	hbridge1forward()
#define gear_reverse() 	hbridge1reverse()
#define gear_stop()		hbridge1stop()

HAL_StatusTypeDef init_gear();
uint8_t gear_number();
bool change_gear();

#endif
