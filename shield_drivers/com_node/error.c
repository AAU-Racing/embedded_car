#include <stm32f4xx_hal.h>

#include <stdbool.h>

#include <board_driver/can.h>

#include "error.h"

#define BUFFER_SIZE 50

typedef struct {
	uint16_t id;
	uint32_t error_id;
} error_msg;

static error_msg buffer[50];
static uint8_t head = 0;
static uint8_t tail = 0;
static uint8_t size = 0;

bool get_error(uint16_t *id, uint32_t *error_id) {
	if (size == 0) {
		return false;
	}

	*id = buffer[tail].id;
	*error_id = buffer[tail].error_id;

	tail = (tail + 1) % BUFFER_SIZE;
	size--;

	return true;
}

void error_callback(CanRxMsgTypeDef* Msg){
	if (size < BUFFER_SIZE) {
		return;
	}

	buffer[head] = (error_msg) { Msg->StdId, Msg->Data[3] << 24 | Msg->Data[2] << 16 | Msg->Data[1] << 8 | Msg->Data[0] };
	head = (head + 1) % BUFFER_SIZE;
	size++;
}

HAL_StatusTypeDef error_init(void){
	return CAN_Filter(CAN_ERROR_HANDLE_ID_START, CAN_ERROR_HANDLE_MASK, error_callback);
}
