#ifndef SEND_DATA_H
#define SEND_DATA_H

#include <stdbool.h>

#include <stm32f4xx_hal.h>

HAL_StatusTypeDef handle_oil_pressure();
HAL_StatusTypeDef handle_gear();
HAL_StatusTypeDef handle_water_temp(bool adc_ready);
HAL_StatusTypeDef handle_current_clamps(bool adc_ready);
HAL_StatusTypeDef handle_wheel_sensor();

#endif
