#ifndef DASHBOARD_BUTTONS_H
#define DASHBOARD_BUTTONS_H

#include <stm32f4xx_hal.h>
#include <stdbool.h>

typedef enum {
	DASHBOARD_BUTTON1,
	DASHBOARD_BUTTON2,
	DASHBOARD_BUTTON3,
	DASHBOARD_BUTTON4,
	DASHBOARD_BUTTON5,
	DASHBOARD_BUTTON6,

	DASHBOARD_BUTTONn // number of buttons
} dashboard_button;

#define BUTTON1_PIN                    GPIO_PIN_8
#define BUTTON1_GPIO_PORT              GPIOE
#define BUTTON1_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

#define BUTTON2_PIN                    GPIO_PIN_1
#define BUTTON2_GPIO_PORT              GPIOB
#define BUTTON2_GPIO_CLK_ENABLE()      __GPIOB_CLK_ENABLE()

#define BUTTON3_PIN                    GPIO_PIN_12
#define BUTTON3_GPIO_PORT              GPIOE
#define BUTTON3_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

#define BUTTON4_PIN                    GPIO_PIN_10
#define BUTTON4_GPIO_PORT              GPIOE
#define BUTTON4_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

#define BUTTON5_PIN                    GPIO_PIN_14
#define BUTTON5_GPIO_PORT              GPIOE
#define BUTTON5_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

#define BUTTON6_PIN                    GPIO_PIN_7
#define BUTTON6_GPIO_PORT              GPIOE
#define BUTTON6_GPIO_CLK_ENABLE()      __GPIOE_CLK_ENABLE()

#define BUTTON_PORT ((GPIO_TypeDef* const []) { \
	BUTTON1_GPIO_PORT, \
	BUTTON2_GPIO_PORT, \
	BUTTON3_GPIO_PORT, \
	BUTTON4_GPIO_PORT, \
	BUTTON5_GPIO_PORT, \
	BUTTON6_GPIO_PORT, \
})

#define BUTTON_PIN ((const uint16_t const []) { \
	BUTTON1_PIN, \
	BUTTON2_PIN, \
	BUTTON3_PIN, \
	BUTTON4_PIN, \
	BUTTON5_PIN, \
	BUTTON6_PIN, \
})

void dashboard_buttons_init(void);
bool dashboard_button_get_state(dashboard_button b, GPIO_PinState* state);
bool is_triggered(dashboard_button b);

#endif /* DASHBOARD_BUTTONS_H */
