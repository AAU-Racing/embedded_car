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

uint8_t handle_oil_pressure() {
    static uint32_t next = 0;
    if (next < HAL_GetTick()) {
        next = HAL_GetTick() + 50;

    	if (CAN_Send(CAN_OIL_PRESSURE, (uint8_t[]) { get_oil_pressure() }, 1) != CAN_OK) {
    	 	return 1;
    	}
    }

	return 0;
}

uint8_t handle_gear() {
    check_gear_change();

	uint8_t changed_result = HAL_OK;
	if (gear_has_changed()) {
		uint8_t gearNumber = gear_number();
		changed_result = CAN_Send(CAN_GEAR_NUMBER, &gearNumber, 1);
	} else if (gear_change_failed()) {
		changed_result = CAN_Send(CAN_GEAR_CHANGE_FAILED, (uint8_t[]) { 1 }, 1);
    }

    static int next = 24;
    if (next < HAL_GetTick()) {
        next = HAL_GetTick() + 20;
    	uint16_t gear_feedback = read_gear_feedback();

    	if (CAN_Send(CAN_GEAR_FEEDBACK, (uint8_t*) &gear_feedback, 2) != CAN_OK) {
            return 2;
        }
    }

	if (changed_result != CAN_OK) {
		return 1;
	}

	return 0;
}

uint8_t handle_water_temp(bool adc_ready) {
    static uint32_t next = 23;
	if (adc_ready) {
        if (next < HAL_GetTick()) {
            next = HAL_GetTick() + 200;

    		uint16_t water_in = read_water_in();
    		uint16_t water_out = read_water_out();

    		uint8_t in_res = CAN_Send(CAN_WATER_TEMPERATURE_IN, (uint8_t*) &water_in, 2);
    		uint8_t out_res = CAN_Send(CAN_WATER_TEMPERATURE_OUT, (uint8_t*) &water_out, 2);

    		if (in_res != CAN_OK && out_res != CAN_OK) {
    			return 2;
    		}
        }

		return 0;
	}

	return 1;
}

HAL_StatusTypeDef handle_current_clamps(bool adc_ready) {
    static uint32_t next = 22;
	if (adc_ready) {
        if (next < HAL_GetTick()) {
            next = HAL_GetTick() + 4;

    		uint16_t battery_current = read_battery_current();
    		uint16_t inverter_current = read_inverter_current();

    		uint8_t in_res = CAN_Send(CAN_BATTERY_CURRENT, (uint8_t*) &battery_current, 2);
    		uint8_t out_res = CAN_Send(CAN_INVERTER_CURRENT, (uint8_t*) &inverter_current, 2);

            if (in_res != CAN_OK && out_res != CAN_OK) {
                return 2;
            }
        }

        return 0;
    }

    return 1;
}

uint8_t handle_wheel_sensor() {
    static uint32_t next = 21;
    if (next < HAL_GetTick()) {
        next = HAL_GetTick() + 20;

    	serializable_float fr = wheel_sensor_fr();
    	serializable_float fl = wheel_sensor_fl();
    	serializable_float rr = wheel_sensor_rr();
    	serializable_float rl = wheel_sensor_rl();

    	uint8_t fr_res = CAN_Send(CAN_WHEEL_SPEED_FR, fr.bytes, 4);
    	uint8_t fl_res = CAN_Send(CAN_WHEEL_SPEED_FL, fl.bytes, 4);
    	uint8_t rr_res = CAN_Send(CAN_WHEEL_SPEED_RR, rr.bytes, 4);
    	uint8_t rl_res = CAN_Send(CAN_WHEEL_SPEED_RL, rl.bytes, 4);

    	if (fr_res != CAN_OK || fl_res != CAN_OK || rr_res != CAN_OK || rl_res != CAN_OK) {
    	 	return 1;
    	}
    }

	return 0;
}
