#include <stm32f4xx_hal.h>

#include <stdbool.h>

#include <board_driver/can.h>

#include "water_temperature.h"

static uint16_t water_temperature_in = 0;
static uint16_t water_temperature_out = 0;
static bool water_in_is_new  = false;
static bool water_out_is_new = false;

bool get_water_temp_in(uint16_t* temp) {
	bool is_new_data = water_in_is_new;
	*temp = water_temperature_in;
	water_in_is_new = false;
	return is_new_data;
}

bool get_water_temp_out(uint16_t* temp) {
	bool is_new_data = water_out_is_new;
	*temp = water_temperature_out;
	water_out_is_new = false;
	return is_new_data;
}

void water_temperature_callback(CAN_RxFrame* Msg) {
	if (Msg->StdId == CAN_WATER_TEMPERATURE_IN) {
		water_temperature_in = (Msg->Msg[1] << 8 | Msg->Msg[0]);
		water_in_is_new = true;
	}
	else if (Msg->StdId == CAN_WATER_TEMPERATURE_OUT) {
		water_temperature_out = (Msg->Msg[1] << 8 | Msg->Msg[0]);
		water_out_is_new = true;
	}
}

HAL_StatusTypeDef water_temp_init(void) {
	return CAN_Filter(CAN_WATER_TEMPERATURE_IN, 0x7FE, water_temperature_callback);
}
