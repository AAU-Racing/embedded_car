#include <stm32f4xx_hal.h>

#include <stdbool.h>

#include <board_driver/can.h>

#include "current_clamps.h"

static uint16_t battery_current = 0;
static uint16_t inverter_current = 0;
static bool battery_current_is_new  = false;
static bool inverter_current_is_new = false;

bool get_battery_current(uint16_t* current) {
	bool is_new_data = battery_current_is_new;
	*current = battery_current;
	battery_current_is_new = false;
	return is_new_data;
}

bool get_inverter_current(uint16_t* current) {
	bool is_new_data = inverter_current_is_new;
	*current = inverter_current;
	inverter_current_is_new = false;
	return is_new_data;
}

void current_callback(CanRxMsgTypeDef* Msg) {
	if (Msg->StdId == CAN_BATTERY_CURRENT) {
		battery_current = (Msg->Data[1] << 8 | Msg->Data[0]);
		battery_current_is_new = true;
	}
	else if (Msg->StdId == CAN_INVERTER_CURRENT) {
		inverter_current = (Msg->Data[1] << 8 | Msg->Data[0]);
		inverter_current_is_new = true;
	}
}

HAL_StatusTypeDef current_clamps_init(void) {
	return CAN_Filter(CAN_BATTERY_CURRENT, 0x7FE, current_callback);
}
