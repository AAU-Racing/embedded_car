#include <stm32f4xx_hal.h>

#include "gear.h"

#include <board_driver/can.h>

static int gear_No = 0;
static int gear_is_new = false;

bool get_gear(gear_t* gear){
	*gear = gear_No;
	bool is_new_data = gear_is_new;
	gear_is_new = false;
	return is_new_data; 
}

void gear_Callback(CanRxMsgTypeDef* Msg){
	gear_No = Msg->Data[0];
	gear_is_new = true;
}

HAL_StatusTypeDef gear_init(void){
	return CAN_Filter(CAN_GEAR_NUMBER, 0x7ff, gear_Callback);
}