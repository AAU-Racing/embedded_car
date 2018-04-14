#include <stm32f4xx_hal.h>
#include <board_driver/can.h>

#include <stdio.h>

#include "gear.h"
#include "gear_feedback.h"
#include "ignition_cut.h"

#define TIMEOUT 500 // ms
#define DEFAULT_POSITION 2048
#define NETURAL_POSITION 1024
#define UP_POSITION 3072
#define	DOWN_POSITION 512
#define EPSILON 10

static uint8_t gear_num = 0;
static volatile uint8_t requested_gear_num = 0;
static volatile uint32_t gear_down_start = 0;
static bool failed_gear_change = false;
static uint16_t gear_feedback = 0;
static bool has_changed = false;

static int within_range(uint16_t number, uint16_t limit) {
	return limit - EPSILON < number && number < limit + EPSILON;
}

static void gear_to_default_position() {
    disable_ignition_cut();

    if (within_range(gear_feedback, DEFAULT_POSITION)) {
        gear_stop();
    }
    else if (gear_feedback < DEFAULT_POSITION) {
        gear_reverse();
    }
    else {
        gear_forward();
    }
}

static void gear_up() {
    printf("gear_up\n");
    enable_ignition_cut();

    if (gear_num == 0) {
        gear_forward();
        if (within_range(gear_feedback, DOWN_POSITION)) {
            has_changed = true;
            gear_num = 1;
            disable_ignition_cut();
        }
	}
	else if (gear_num != 6) {
        gear_reverse();
        if (within_range(gear_feedback, UP_POSITION)) {
            has_changed = true;
            gear_num++;
            disable_ignition_cut();
        }
	}
}

static void gear_down() {
    printf("gear_up\n");
    if (HAL_GetTick() > gear_down_start + TIMEOUT) {
        requested_gear_num = gear_num;
        failed_gear_change = true;
        gear_to_default_position();
        return;
    }

    enable_ignition_cut();

    if (gear_num == 1) {
        gear_reverse();
        if (within_range(gear_feedback, NETURAL_POSITION)) {
            has_changed = true;
            gear_num = 0;
            disable_ignition_cut();
        }
	}
	else if (gear_num != 0) {
        gear_forward();
        if (within_range(gear_feedback, DOWN_POSITION)) {
            has_changed = true;
            gear_num--;
            disable_ignition_cut();
        }
	}
}

static void gear_callback(CAN_RxFrame *msg) {
	if (msg->Msg[0] == CAN_GEAR_BUTTON_UP) {
		if (requested_gear_num < 6) {
			requested_gear_num++;
		}
	}
	else if (msg->Msg[0] == CAN_GEAR_BUTTON_DOWN) {
		if (requested_gear_num > 0) {
			requested_gear_num--;
            gear_down_start = HAL_GetTick();
		}
	}
}

// Public functions
uint8_t init_gear() {
	init_gear_feedback();
	return CAN_Filter(CAN_GEAR_BUTTONS, 0x7FF, gear_callback);
}

void check_gear_change() {
    gear_feedback = read_gear_feedback();

	if (requested_gear_num > gear_num) {
		gear_up();
	}
	else if (requested_gear_num < gear_num) {
		gear_down();
	}
    else {
        gear_to_default_position();
    }
}

uint8_t gear_number() {
	return gear_num;
}

bool gear_has_changed() {
    bool value = has_changed;
    has_changed = false;
    return value;
}

bool gear_change_failed() {
    bool value = failed_gear_change;
    failed_gear_change = false;
    return value;
}
