#include <stm32f4xx_hal.h>

#include <stdbool.h>

#include "oil.h"

#include <board_driver/can.h>

static bool oil_pressure_is_ok = false;
static bool data_is_new = false;

bool oilPressure_OK(bool* ok){
	*ok = oil_pressure_is_ok;
	bool it_is_new = data_is_new;
	data_is_new = false;
	return it_is_new;
}

void oil_pressure_Callback(CanRxMsgTypeDef* Msg){
	oil_pressure_is_ok = Msg->Data[0];
	data_is_new = true;
}

HAL_StatusTypeDef oil_init(void){
	return CAN_Filter(CAN_OIL_PRESSURE, 0x7FF, oil_pressure_Callback);
}