#include <math.h>
#include <stm32f4xx.h>

#include <board_driver/can.h>

#include "sensor_data.h"

static int neutral_state = 0;
static bool neutral_is_new = false;

static float water_in = 0;
static bool water_in_is_new = false;

static float water_out = 0;
static bool water_out_is_new = false;

bool get_neutral(bool* neutral) {
	*neutral = neutral_state;
	bool is_new_data = neutral_is_new;
	neutral_is_new = false;
	return is_new_data;
}

bool get_water_in(float* value) {
	*value = water_in;
	bool is_new_data = water_in_is_new;
	water_in_is_new = false;
	return is_new_data;
}

bool get_water_out(float* value) {
	*value = water_out;
	bool is_new_data = water_out_is_new;
	water_out_is_new = false;
	return is_new_data;
}

static float convert_water_temp(uint16_t data) {
	return -27.271952718 * log(10000 * data / (4095.0 - data)) + 240.1851825535;
}

void sensor_data_callback(CAN_RxFrame* msg) {
	if (msg->StdId == CAN_NEUTRAL_SWITCH) {
		neutral_state = msg->Msg[0];
		neutral_is_new = true;
	}
	else if (msg->StdId == CAN_WATER_TEMPERATURE_IN) {
		uint16_t value = msg->Msg[0] << 8 | msg->Msg[1];
		water_in = convert_water_temp(value);
		water_in_is_new = true;
	}
	else if (msg->StdId == CAN_WATER_TEMPERATURE_OUT) {
		uint16_t value = msg->Msg[0] << 8 | msg->Msg[1];
		water_out = convert_water_temp(value);
		water_out_is_new = true;
	}
}

int sensor_data_init(void){
	return can_filter(CAN_SENSOR_DATA_ID_START, CAN_SENSOR_DATA_MASK, sensor_data_callback);
}
