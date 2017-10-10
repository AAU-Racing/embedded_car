#include <stdbool.h>

#include <stm32f4xx_hal.h>
#include <board_driver/adc.h>

#include "gear_feedback.h"

static uint8_t channel_rank;

HAL_StatusTypeDef init_gear_feedback() {
	ADC_GEAR_CHANNEL_GPIO_CLK_ENABLE();

	init_analog_pins(ADC_GEAR_CHANNEL_GPIO_PORT, ADC_GEAR_CHANNEL_PIN);

	if (init_adc_channel(ADC_GEAR_CHANNEL, &channel_rank) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint16_t read_gear_feedback() {
    return read_adc_value(channel_rank);
}
