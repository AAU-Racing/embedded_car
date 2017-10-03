#include <stdbool.h>

#include <stm32f4xx_hal.h>
#include <board_driver/adc.h>

#include "water_temp.h"

uint8_t channel_rank[2];

HAL_StatusTypeDef init_water_temp() {
	ADCx1_CHANNEL_GPIO_CLK_ENABLE();
	ADCx2_CHANNEL_GPIO_CLK_ENABLE();

/*
	The following code course errors.
	It looks incomplete. Get it fixed!
*//*
	init_analog_pins(ADCx1_CHANNEL_GPIO_PORT, ADCx1_CHANNEL_PIN);
	init_analog_pins(ADCx2_CHANNEL_GPIO_PORT, ADCx2_CHANNEL_PIN);

	if (init_adc_channel(ADCx1_CHANNEL, channel_rank) != HAL_OK) {
		return HAL_ERROR;
	}

	if (init_adc_channel(ADCx2_CHANNEL, channel_rank + 1) != HAL_OK) {
		return HAL_ERROR;
	}
*/

	return HAL_OK;
}

int read_water_in() {
	//return read_adc_value(channel_rank[0]);
	return 0;
}

int read_water_out() {
	//return read_adc_value(channel_rank[1]);
	return 0;
}
