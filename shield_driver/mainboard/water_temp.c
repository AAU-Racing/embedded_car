#include <stdbool.h>

#include <stm32f4xx_hal.h>
#include <board_driver/adc.h>

#include "water_temp.h"

static uint8_t channel_rank[2];

HAL_StatusTypeDef init_water_temp() {
	ADC_WATER_IN_CHANNEL_GPIO_CLK_ENABLE();
	ADC_WATER_OUT_CHANNEL_GPIO_CLK_ENABLE();

	init_analog_pins(ADC_WATER_IN_CHANNEL_GPIO_PORT, ADC_WATER_IN_CHANNEL_PIN);
	init_analog_pins(ADC_WATER_OUT_CHANNEL_GPIO_PORT, ADC_WATER_OUT_CHANNEL_PIN);

	if (init_adc_channel(ADC_WATER_IN_CHANNEL, channel_rank) != HAL_OK) {
		return HAL_ERROR;
	}

	if (init_adc_channel(ADC_WATER_OUT_CHANNEL, channel_rank + 1) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint16_t read_water_in() {
    return read_adc_value(channel_rank[0]);
}

uint16_t read_water_out() {
    return read_adc_value(channel_rank[1]);
}
