#include <stm32f4xx_hal.h>
#include <stdint.h>
#include <stdbool.h>

#include "gpio.h"

// Allow different external interrupt lines to be initialized
#define EXTI_IRQn ((const uint8_t const []) { \
	EXTI0_IRQn, \
	EXTI1_IRQn, \
	EXTI2_IRQn, \
	EXTI3_IRQn, \
	EXTI4_IRQn, \
	EXTI9_5_IRQn, \
	EXTI15_10_IRQn, \
})

static int get_interrupt_line(uint32_t pin) {
	switch(pin) {
		case GPIO_PIN_0:
			return 0;
		case GPIO_PIN_1:
			return 1;
		case GPIO_PIN_2:
			return 2;
		case GPIO_PIN_3:
			return 3;
		case GPIO_PIN_4:
			return 4;
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
			return 5;
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			return 6;
		default:
			return -1;
	}
}

// Store all pins initialzed as normal
typedef struct {
	GPIO_TypeDef *Port;
	uint32_t Pin;
} gpio_pin;

// And store them as a stack (which only grows)
gpio_pin input_pins[25];
gpio_pin output_pins[25];
uint8_t count_input = 0;
uint8_t count_output = 0;

// Store callback functions
gpio_callback callback_funcions[16];

// Init functions
void gpio_input_init(GPIO_TypeDef *port, uint32_t pin) {
	HAL_GPIO_Init(port, &(GPIO_InitTypeDef) {
		.Pin = pin,
		.Mode = GPIO_MODE_INPUT,
		.Pull = GPIO_PULLDOWN,
		.Speed = GPIO_SPEED_FAST,
	});

	input_pins[count_input++] = (gpio_pin) {
		.Port = port,
		.Pin = pin,
	};
}

void gpio_exti_init(GPIO_TypeDef *port, uint32_t pin, uint32_t mode, gpio_callback callback) {
	HAL_GPIO_Init(port, &(GPIO_InitTypeDef) {
		.Pin = pin,
		.Mode = mode,
		.Pull = GPIO_NOPULL,
	});

	HAL_NVIC_SetPriority(EXTI_IRQn[get_interrupt_line(pin)], 0xF, 0xF);
	HAL_NVIC_EnableIRQ(EXTI_IRQn[get_interrupt_line(pin)]);

	callback_funcions[pin] = callback;
}

void gpio_output_init(GPIO_TypeDef *port, uint32_t pin) {
	HAL_GPIO_Init(port, &(GPIO_InitTypeDef) {
		.Pin = pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_PULLUP,
		.Speed = GPIO_SPEED_HIGH,
	});

	output_pins[count_output++] = (gpio_pin) {
		.Port = port,
		.Pin = pin,
	};
}

bool gpio_toogle_on(GPIO_TypeDef *port, uint32_t pin) {
	for (int i = 0; i < count_output; i++) {
		if (port == output_pins[i].Port && pin == output_pins[i].Pin) {
			HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
			return true;
		}
	}

	return false;
}

bool gpio_toogle_off(GPIO_TypeDef *port, uint32_t pin) {
	for (int i = 0; i < count_output; i++) {
		if (port == output_pins[i].Port && pin == output_pins[i].Pin) {
			HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
			return true;
		}
	}

	return false;
}

// Get state of a normal pin
bool gpio_get_state(GPIO_TypeDef *port, uint32_t pin, GPIO_PinState* state) {
	for (int i = 0; i < count_input; i++) {
		if (port == input_pins[i].Port && pin == input_pins[i].Pin) {
			*state = HAL_GPIO_ReadPin(port, pin);
			return true;
		}
	}

	return false;
}

// Handle interrupts
void EXTI0_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void EXTI1_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

void EXTI3_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

void EXTI4_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

void EXTI15_10_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

// Interrupt callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	callback_funcions[GPIO_Pin](GPIO_Pin);
}
