#include <stdbool.h>

#include <stm32f4xx_hal.h>
#include <board_driver/adc.h>

#include "brake_pressure.h"

uint8_t channel_rank[2];

HAL_StatusTypeDef init_brake_pressure() {
	ADC_BRAKE1_CHANNEL_GPIO_CLK_ENABLE();
	ADC_BRAKE2_CHANNEL_GPIO_CLK_ENABLE();

	init_analog_pins(ADC_BRAKE1_CHANNEL_GPIO_PORT, ADC_BRAKE1_CHANNEL_PIN);
	init_analog_pins(ADC_BRAKE2_CHANNEL_GPIO_PORT, ADC_BRAKE2_CHANNEL_PIN);

	if (init_adc_channel(ADC_BRAKE1_CHANNEL, channel_rank) != HAL_OK) {
		return HAL_ERROR;
	}

	if (init_adc_channel(ADC_BRAKE2_CHANNEL, channel_rank + 1) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

int read_brake_pressure_front() {
    return read_adc_value(channel_rank[0]);
}

int read_brake_pressure_rear() {
    return read_adc_value(channel_rank[1]);
}
