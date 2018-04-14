#include <board_driver/can.h>
#include <shield_drivers/traction_control/water_temp.h>
#include <shield_drivers/traction_control/current_clamps.h>
#include <shield_drivers/traction_control/wheel_sensor.h>
#include <shield_drivers/traction_control/gear.h>
#include <shield_drivers/traction_control/gear_feedback.h>
#include <shield_drivers/traction_control/oil_pressure.h>
#include <shield_drivers/traction_control/hbridge.h>

#include <stdio.h>

#include "send_data.h"

HAL_StatusTypeDef handle_oil_pressure() {
	return CAN_Send(CAN_OIL_PRESSURE, (uint8_t[]) { get_oil_pressure() }, 1);
}

HAL_StatusTypeDef handle_gear() {
    check_gear_change();

	HAL_StatusTypeDef changed_result = HAL_OK;
	if (gear_has_changed()) {
		uint8_t gearNumber = gear_number();
		changed_result = CAN_Send(CAN_GEAR_NUMBER, &gearNumber, 1);
	} else if (gear_change_failed()) {
		changed_result = CAN_Send(CAN_GEAR_CHANGE_FAILED, (uint8_t[]) { 1 }, 1);
    }

	uint16_t gear_feedback = read_gear_feedback();

	HAL_StatusTypeDef gear_feedback_result = CAN_Send(CAN_GEAR_FEEDBACK, &gear_feedback, 2);

	if (gear_feedback_result != HAL_OK || changed_result != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

HAL_StatusTypeDef handle_water_temp(bool adc_ready) {
	if (adc_ready)
	{
		uint16_t water_in = read_water_in();
		uint16_t water_out = read_water_out();

        printf("Water temperature: (%4d, %4d)\n", water_in, water_out);

		HAL_StatusTypeDef in_res = CAN_Send(CAN_WATER_TEMPERATURE_IN, (uint8_t[]) { water_in & 0xFF, (water_in >> 8) & 0xFF }, 2);
		HAL_StatusTypeDef out_res = CAN_Send(CAN_WATER_TEMPERATURE_OUT, (uint8_t[]) { water_out & 0xFF, (water_out >> 8) & 0xFF }, 2);

		if (in_res != HAL_OK && out_res != HAL_OK) {
			return HAL_ERROR;
		}

		return HAL_OK;
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef handle_current_clamps(bool adc_ready) {
	if (adc_ready)
	{
		uint16_t battery_current = read_battery_current();
		uint16_t inverter_current = read_inverter_current();

		HAL_StatusTypeDef in_res = CAN_Send(CAN_BATTERY_CURRENT, (uint8_t[]) { battery_current & 0xFF, (battery_current >> 8) & 0xFF }, 2);
		HAL_StatusTypeDef out_res = CAN_Send(CAN_INVERTER_CURRENT, (uint8_t[]) { inverter_current & 0xFF, (inverter_current >> 8) & 0xFF }, 2);

		if (in_res != HAL_OK && out_res != HAL_OK) {
			return HAL_ERROR;
		}

		return HAL_OK;
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef handle_wheel_sensor() {
	serializable_float fr = wheel_sensor_fr();
	serializable_float fl = wheel_sensor_fl();
	serializable_float rr = wheel_sensor_br();
	serializable_float rl = wheel_sensor_bl();

	HAL_StatusTypeDef fr_res = CAN_Send(CAN_WHEEL_SPEED_FR, fr.bytes, 4);
	HAL_StatusTypeDef fl_res = CAN_Send(CAN_WHEEL_SPEED_FL, fl.bytes, 4);
	HAL_StatusTypeDef rr_res = CAN_Send(CAN_WHEEL_SPEED_RR, rr.bytes, 4);
	HAL_StatusTypeDef rl_res = CAN_Send(CAN_WHEEL_SPEED_RL, rl.bytes, 4);

	if (fr_res != HAL_OK || fl_res != HAL_OK || rr_res != HAL_OK || rl_res != HAL_OK) {
	 	return HAL_ERROR;
	}

	return HAL_OK;
}
