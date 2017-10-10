#include <stm32f4xx_hal.h>

#include <stdbool.h>
#include <string.h>

#include <board_driver/can.h>
#include <shield_drivers/traction_control/wheel_sensor.h>

#include "water_temperature.h"

static serializable_float wheel_speed_FL;
static serializable_float wheel_speed_FR;
static serializable_float wheel_speed_RL;
static serializable_float wheel_speed_RR;

static bool FL_is_new  = false;
static bool FR_is_new  = false;
static bool RL_is_new  = false;
static bool RR_is_new  = false;

bool get_wheel_speed_FL(float* speed){
	bool is_new_data = FL_is_new;
	*speed = wheel_speed_FL.fl;
	FL_is_new = false;
	return is_new_data;
}

bool get_wheel_speed_FR(float* speed){
	bool is_new_data = FR_is_new;
	*speed = wheel_speed_FR.fl;
	FR_is_new = false;
	return is_new_data;
}

bool get_wheel_speed_RL(float* speed){
	bool is_new_data = RL_is_new;
	*speed = wheel_speed_RL.fl;
	RL_is_new = false;
	return is_new_data;
}

bool get_wheel_speed_RR(float* speed){
	bool is_new_data = RR_is_new;
	*speed = wheel_speed_RR.fl;
	RR_is_new = false;
	return is_new_data;
}

void wheel_speed_callback(CanRxMsgTypeDef* Msg){
	if (Msg->StdId == CAN_WHEEL_SPEED_FL){
		memcpy(wheel_speed_FL.bytes, Msg->Data, 4);
		FL_is_new = true;
	}
	else if(Msg->StdId == CAN_WHEEL_SPEED_FR){
		memcpy(wheel_speed_FR.bytes, Msg->Data, 4);
		FR_is_new = true;
	}
	else if(Msg->StdId == CAN_WHEEL_SPEED_RL){
		memcpy(wheel_speed_RL.bytes, Msg->Data, 4);
		RL_is_new = true;
	}
	else if(Msg->StdId == CAN_WHEEL_SPEED_RR){
		memcpy(wheel_speed_RR.bytes, Msg->Data, 4);
		RR_is_new = true;
	}
}

HAL_StatusTypeDef wheel_speed_init(void){
	return CAN_Filter(CAN_WHEEL_SPEED_RL, 0x7fc, wheel_speed_callback);
}