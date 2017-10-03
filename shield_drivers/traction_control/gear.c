#include <stdbool.h>

#include <board_driver/can.h>

#include "gear.h"
#include "gear_feedback.h"

#define FORWARD true
#define REVERSE false
#define TIMEOUT 1000
#define DEFAULT_POS 2048
#define NETURAL_POSITION 1024
#define UP_POSITION 3072
#define	DOWN_POSITION 512

static uint8_t gearNumber = 0;
static uint8_t wantedGearNumber = 0;

static int withinRange(uint16_t number, uint16_t limit) {
	return number < limit + 10 && number > limit - 10;
}

static bool turn(bool returnDirection, uint16_t limit, uint16_t timeout) {
	// Move the gear back into default position
	if (returnDirection) { // is return direction forward?
		gear_forward();
	}
	else {
		gear_reverse();
	}

	bool defaultPos = false;
	uint32_t start = HAL_GetTick();

	// Wait until gear motor is at right limit
	do {
		// Read gear motor position
		uint16_t gearFeedback = read_gear_feedback();

		// Withing range
		if (withinRange(gearFeedback, limit)) {
			defaultPos = true;
		}

		// The driver might not have released the throttle or pushed the clutch
		if (timeout > 0 && HAL_GetTick() - start > timeout) {
			break;
		}
	} while(defaultPos);

	return defaultPos;
}

static bool turnWithReturn(bool returnDirection, uint16_t limit) {
	// Turn to wanted position
	bool shifted = turn(returnDirection, limit, TIMEOUT);

	// Return the gear back into default position
	turn(!returnDirection, DEFAULT_POS, 0);

	gear_stop();

	return shifted;
}

// Callback functions for gear buttons via CAN
static bool gearUp() {
	if (gearNumber == 6) {
		return false;
	}

	bool shifted = false;

	if (gearNumber == 0) {
		shifted = turnWithReturn(FORWARD, DOWN_POSITION);
	}
	else {
		shifted = turnWithReturn(REVERSE, UP_POSITION);
	}

	if (shifted) {
		gearNumber++;
	}

	return shifted;
}

static bool gearDown() {
	if (gearNumber == 0) {
		return false;
	}

	bool shifted = false;

	if (gearNumber == 1) {
		shifted = turnWithReturn(REVERSE, NETURAL_POSITION);
	}
	else {
		shifted = turnWithReturn(FORWARD, DOWN_POSITION);
	}

	if (shifted) {
		gearNumber--;
	}

	return shifted;
}

static void GearCallback(CanRxMsgTypeDef *msg) {
	if (msg->Data[0] == CAN_GEAR_BUTTON_UP) {
		if (wantedGearNumber < 6) {
			wantedGearNumber++;
		}
	}
	else if (msg->Data[0] == CAN_GEAR_BUTTON_DOWN) {
		if (wantedGearNumber > 0) {
			wantedGearNumber--;
		}
	}
}

// Public functions
HAL_StatusTypeDef init_gear() {
	init_gear_feedback();
	return CAN_Filter(CAN_GEAR_BUTTONS, 0x7FF, GearCallback);
}

bool change_gear() {
	if (wantedGearNumber == gearNumber) {
		return false;
	}
	else if (wantedGearNumber > gearNumber) {
		return gearUp();
	}
	else if (wantedGearNumber < gearNumber) {
		return gearDown();
	}
	else {
		return false;
	}
}

uint8_t gear_number() {
	return gearNumber;
}
