#ifndef GEAR_FEEDBACK_H
#define GEAR_FEEDBACK_H

#include <stm32f4xx_hal.h>

#define ADC_GEAR_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()
#define ADC_GEAR_CHANNEL_PIN                GPIO_PIN_1
#define ADC_GEAR_CHANNEL_GPIO_PORT          GPIOB
#define ADC_GEAR_CHANNEL                    ADC_CHANNEL_9

uint8_t init_gear_feedback();
uint16_t read_gear_feedback();

#endif
