#include <stm32f4xx_hal.h>

#include <board_driver/i2c.h>

#include "TLC59116.h"

#define ARR_SIZE(a) (sizeof(a)/sizeof(a[0]))

static bool china_led = false;

enum registers {
	MODE1, MODE2,

	PWM0, PWM1, PWM2, PWM3, PWM4, PWM5, PWM6, PWM7,
	PWM8, PWM9, PWM10, PWM11, PWM12, PWM13, PWM14, PWM15,

	GRPPWM,
	GRPFREQ,

	LEDOUT0, LEDOUT1, LEDOUT2, LEDOUT3,
	SUBADR1, SUBADR2, SUBADR3,

	ALLCALLADR,
	IREF,

	EFLAG1, EFLAG2,
};

// The hardware selectable pins from the datasheet
#define A3	0
#define A2	0
#define A1	0
#define A0	0

#define ADDR ((1 << 7) | (1 << 6) | (0 << 5) | (A3 << 4) | (A2 << 3) | (A1 << 2) | (A0 << 1)) // Not used

// Warning because we shift here you cannot just iterate on the address
enum address {
	ADDR1 = 0x60 << 1, // Shifted once to make room for the RW bit
	ADDR2 = 0x61 << 1,
	ADDR3 = 0x62 << 1,
	ADDR4 = 0x63 << 1,
};

#define controller_lut(x) ((enum address[]){ADDR1,ADDR2,ADDR3,ADDR4}[(x)])

#define N_LEDS_PR_CONTROLLER	(5)

/*
#define R_ADDR	(ADDR | (1 << 0))
#define W_ADDR	(ADDR | (0 << 0))
*/


static void write_cmd(enum address addr, enum registers reg, uint8_t value) {
	uint8_t p[] = {(uint8_t)reg, value};
	if (i2c_master_transmit_DMA(addr, p, ARR_SIZE(p)) != 0) {
		while(1); // TODO remove
	}
}

static void set_rgb(enum address addr, unsigned led, unsigned r, unsigned g, unsigned b) {
	enum registers led_lookup[] = {
		PWM0, PWM3, PWM6,
		PWM9, PWM12, PWM15,
	};

	const enum registers pwm_reg = led_lookup[led]; //PWM0 + (led*3);

	const enum registers reg_b = pwm_reg + 0;
	const enum registers reg_g = pwm_reg + 1;
	const enum registers reg_r = pwm_reg + 2;

	write_cmd(addr, reg_r, china_led ? g : r);
	write_cmd(addr, reg_g, china_led ? r : g);
	write_cmd(addr, reg_b, b);
}

static void init_reset_pin(void) {
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = GPIO_PIN_3;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL; //GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(1); // We need to wait for the pin to switch state
}

static void init_controller(enum address ctrl) {
	write_cmd(ctrl, MODE1, 0x00);
	write_cmd(ctrl, MODE2, 0x00);

	for (enum registers led_reg = LEDOUT0; led_reg <= LEDOUT3; led_reg++) {
		write_cmd(ctrl, led_reg, 0xFF);
	}

	for (enum registers pwm_reg = PWM0; pwm_reg <= PWM15; pwm_reg++) {
		write_cmd(ctrl, pwm_reg, 0x00);
	}

}

void set_led(unsigned led, unsigned r, unsigned g, unsigned b) {
	set_rgb(controller_lut((led % 20) / N_LEDS_PR_CONTROLLER), led % N_LEDS_PR_CONTROLLER, r, g, b);
}

int led_driver_init(bool china) {
	china_led = china;
	// Handle reset pin
	init_reset_pin();

	if (i2c_init() != 0) {
		while(1);
		return -1;
	}

	init_controller(ADDR1);
	init_controller(ADDR2);
	init_controller(ADDR3);
	init_controller(ADDR4);

	return 0;
}
