#ifndef HBRIDGE_H
#define HBRIDGE_H

#include <stm32f4xx_hal.h>

#define HBRIDGE_PWM_TIMER				TIM4
#define HBRIDGE_PWM_TIMER_CLK_ENABLE()  __HAL_RCC_TIM4_CLK_ENABLE()

#define HBRIDGE1_EN_PIN					PIN_11
#define HBRIDGE1_EN_PORT				GPIOD

#define HBRIDGE1_DIR_PIN				PIN_10
#define HBRIDGE1_DIR_PORT				GPIOD

#define HBRIDGE1_PWM_PIN				GPIO_PIN_13
#define HBRIDGE1_PWM_PORT				GPIOD
#define HBRIDGE1_PWM_CLK_ENABLE()		__HAL_RCC_GPIOD_CLK_ENABLE()
#define HBRIDGE1_PWM_CHANNEL			TIM_CHANNEL_2
#define HBRIDGE1_PWM_GPIO_AF			GPIO_AF2_TIM4


#define HBRIDGE2_EN_PIN					PIN_14
#define HBRIDGE2_EN_PORT				GPIOD

#define HBRIDGE2_DIR_PIN				PIN_15
#define HBRIDGE2_DIR_PORT				GPIOD

#define HBRIDGE2_PWM_PIN				GPIO_PIN_12
#define HBRIDGE2_PWM_PORT				GPIOD
#define HBRIDGE2_PWM_CLK_ENABLE()		__HAL_RCC_GPIOD_CLK_ENABLE()
#define HBRIDGE2_PWM_CHANNEL			TIM_CHANNEL_1
#define HBRIDGE2_PWM_GPIO_AF			GPIO_AF2_TIM4

void init_hbridge();
void hbridge1forward();
void hbridge1reverse();
void hbridge1stop();
void hbridge2forward();
void hbridge2reverse();
void hbridge2stop();

#endif
