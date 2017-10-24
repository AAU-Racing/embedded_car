#ifndef STEERING_WHEEL_BUTTONS_H
#define STEERING_WHEEL_BUTTONS_H

#include <stm32f4xx_hal.h>
#include <stdbool.h>

typedef enum {
	SW_BUTTON1,
	SW_BUTTON2,
	SW_BUTTON3,
	SW_BUTTON4,
	SW_BUTTON5,

	SW_BUTTONn // number of buttons
} sw_button;


#define SW_BUTTON1_PIN                    PIN_13
#define SW_BUTTON1_GPIO_PORT              GPIOB

#define SW_BUTTON2_PIN                    PIN_15
#define SW_BUTTON2_GPIO_PORT              GPIOB

#define SW_BUTTON3_PIN                    PIN_11
#define SW_BUTTON3_GPIO_PORT              GPIOD

#define SW_BUTTON4_PIN                    PIN_6
#define SW_BUTTON4_GPIO_PORT              GPIOC

#define SW_BUTTON5_PIN                    PIN_9
#define SW_BUTTON5_GPIO_PORT              GPIOD


#define SW_BUTTON_PORT ((GPIO_TypeDef* const []) { \
	SW_BUTTON1_GPIO_PORT, \
	SW_BUTTON2_GPIO_PORT, \
	SW_BUTTON3_GPIO_PORT, \
	SW_BUTTON4_GPIO_PORT, \
	SW_BUTTON5_GPIO_PORT, \
})

#define SW_BUTTON_PIN ((const uint16_t const []) { \
	SW_BUTTON1_PIN, \
	SW_BUTTON2_PIN, \
	SW_BUTTON3_PIN, \
	SW_BUTTON4_PIN, \
	SW_BUTTON5_PIN, \
})

void sw_buttons_init(void);

#endif /* STEERING_WHEEL_BUTTONS_H */
