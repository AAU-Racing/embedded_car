#include <stm32f4xx_hal.h>
#include <board_driver/can.h>

#include "gear.h"
#include "gear_feedback.h"
#include "ignition_cut.h"

#define TIMEOUT 500 // ms
#define DEFAULT_POSITION 2048
#define NETURAL_POSITION 1024
#define UP_POSITION 3072
#define	DOWN_POSITION 512
#define EPSILON 10

static uint8_t gearNumber = 0;
static volatile uint8_t wantedGearNumber = 0;
static volatile uint32_t gearDownStart = 0;
static bool failedGearChange = false;
static uint16_t gearFeedback = 0;

static int withinRange(uint16_t number, uint16_t limit) {
	return limit - EPSILON < number && number < limit + EPSILON;
}

static void gearUp() {
    enable_ignition_cut();

    if (gearNumber == 0) {
        gear_forward();
        if (withinRange(gearFeedback, DOWN_POSITION)) {
            gearNumber = 1;
            disable_ignition_cut();
        }
	}
	else if (gearNumber != 6) {
        gear_reverse();
        if (withinRange(gearFeedback, UP_POSITION)) {
            gearNumber++;
            disable_ignition_cut();
        }
	}
}

static void gearDown() {
    if (HAL_GetTick() > gearDownStart + TIMEOUT) {
        wantedGearNumber = gearNumber;
        failedGearChange = true;
        gearToDefaultPos();
        return;
    }

    enable_ignition_cut();

    if (gearNumber == 1) {
        gear_reverse();
        if (withinRange(gearFeedback, NETURAL_POSITION)) {
            gearNumber = 0;
            disable_ignition_cut();
        }
	}
	else if (gearNumber != 0) {
        gear_forward();
        if (withinRange(gearFeedback, DOWN_POSITION)) {
            gearNumber--;
            disable_ignition_cut();
        }
	}
}

static void gearToDefaultPos() {
    disable_ignition_cut();

    if (withinRange(gearFeedback, DEFAULT_POSITION)) {
        gear_stop();
    }
    else if (gearFeedback < DEFAULT_POSITION) {
        gear_reverse();
    }
    else {
        gear_forward();
    }
}

static void GearCallback(CAN_RxFrame *msg) {
	if (msg->Msg[0] == CAN_GEAR_BUTTON_UP) {
		if (wantedGearNumber < 6) {
			wantedGearNumber++;
		}
	}
	else if (msg->Msg[0] == CAN_GEAR_BUTTON_DOWN) {
		if (wantedGearNumber > 0) {
			wantedGearNumber--;
            gearDownStart = HAL_GetTick();
		}
	}
}

// Public functions
uint8_t init_gear() {
	init_gear_feedback();
	return CAN_Filter(CAN_GEAR_BUTTONS, 0x7FF, GearCallback);
}

void check_gear_change() {
    gearFeedback = read_gear_feedback();

	if (wantedGearNumber > gearNumber) {
		gearUp();
	}
	else if (wantedGearNumber < gearNumber) {
		gearDown();
	}
    else {
        gearToDefaultPos();
    }
}

uint8_t gear_number() {
	return gearNumber;
}

bool gear_change_failed() {
    bool value = failedGearChange;
    failedGearChange = false;
    return value;
}
