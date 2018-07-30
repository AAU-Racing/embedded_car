#include <stm32f4xx.h>

#include <board_driver/can.h>

#include "neutral.h"

static int neutral_state = 0;
static int neutral_is_new = false;

bool get_neutral(bool* neutral) {
	*neutral = neutral_state;
	bool is_new_data = neutral_is_new;
	neutral_is_new = false;
	return is_new_data;
}

void neutral_callback(CAN_RxFrame* Msg) {
	neutral_state = Msg->Msg[0];
	neutral_is_new = true;

	can_transmit(7, (uint8_t[]) { 1 }, 1);
}

int neutral_init(void){
	return can_filter(CAN_NEUTRAL_SWITCH, 0x7ff, neutral_callback);
}
