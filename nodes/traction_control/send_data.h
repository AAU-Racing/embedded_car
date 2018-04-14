#ifndef SEND_DATA_H
#define SEND_DATA_H

#include <stdbool.h>

#include <stm32f4xx_hal.h>

uint8_t handle_oil_pressure();
uint8_t handle_gear();
uint8_t handle_water_temp(bool adc_ready);
uint8_t handle_current_clamps(bool adc_ready);
uint8_t handle_wheel_sensor();

#endif
