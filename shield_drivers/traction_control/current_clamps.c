#include <stdbool.h>

#include <stm32f4xx_hal.h>
#include <board_driver/adc.h>
#include <board_driver/can.h>

#include "current_clamps.h"

static uint8_t channel_rank[2];

HAL_StatusTypeDef init_current_clamps() {
	ADC_CURRENT1_CHANNEL_GPIO_CLK_ENABLE();
	ADC_CURRENT2_CHANNEL_GPIO_CLK_ENABLE();

	init_analog_pins(ADC_CURRENT1_CHANNEL_GPIO_PORT, ADC_CURRENT1_CHANNEL_PIN);
	init_analog_pins(ADC_CURRENT2_CHANNEL_GPIO_PORT, ADC_CURRENT2_CHANNEL_PIN);

	if (init_adc_channel(ADC_CURRENT1_CHANNEL, channel_rank) != HAL_OK) {
		return HAL_ERROR;
	}

	if (init_adc_channel(ADC_CURRENT2_CHANNEL, channel_rank + 1) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint16_t read_battery_current() {
    return read_adc_value(channel_rank[0]);
}

uint16_t read_inverter_current() {
    return read_adc_value(channel_rank[1]);
}
