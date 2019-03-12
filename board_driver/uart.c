#include <stdio.h>
#include <stdint.h>
#include <stm32f4xx.h>

#include "gpio.h"
#include "uart.h"
#include "ringbuffer.h"

#define BUF_SIZE 1024
#define CHANNELS 3
#define CONFIGS 8
#define INSTANCES 4


#define UART_INSTANCE ((USART_TypeDef* const []) { \
    USART1,	\
    USART6, \
    USART3, \
    USART2, \
    USART6, \
	USART6, \
	USART2, \
	USART1  \
})

#define UART_RX_PIN ((const GPIO_Pin const []) { \
    PIN_7,  \
    PIN_7,  \
    PIN_11, \
    PIN_6,  \
    PIN_7,  \
	PIN_7,  \
	PIN_6,  \
	PIN_7   \
})

#define UART_RX_PORT ((GPIO_TypeDef* const []) { \
    GPIOB, \
    GPIOC, \
    GPIOB, \
    GPIOD, \
    GPIOC, \
	GPIOC, \
	GPIOD, \
	GPIOB \
})

#define UART_TX_PIN ((const GPIO_Pin const []) { \
    PIN_6,  \
    PIN_6,  \
    PIN_10,	\
    PIN_5,  \
    PIN_6,  \
	PIN_6,  \
	PIN_5,  \
	PIN_6  \
})

#define UART_TX_PORT ((GPIO_TypeDef* const []) { \
    GPIOB, \
    GPIOC, \
    GPIOB, \
    GPIOD, \
    GPIOC, \
	GPIOC, \
	GPIOD, \
	GPIOB  \
})

#define UART_AF ((const GPIO_Pin const []) { \
    GPIO_AF7, \
    GPIO_AF8, \
    GPIO_AF7, \
    GPIO_AF7, \
    GPIO_AF8, \
	GPIO_AF8, \
	GPIO_AF7, \
	GPIO_AF7  \
})

typedef struct {
	ringbuffer_t rb_tx;
	ringbuffer_t rb_rx;
	uint8_t buffer_tx[BUF_SIZE];
	uint8_t buffer_rx[BUF_SIZE];
} uart_channel;

uart_channel channels[CHANNELS];

uint8_t channel_configs[CONFIGS] = { 255, 255, 255, 255, 255, 255, 255, 255 };
uint8_t current_channel = 0;
uint8_t instance_channels[INSTANCES] = {255, 255, 255, 255};


void uart_init(uint8_t config) {

	channel_configs[config] = current_channel++;
	uart_channel* channel = channels + channel_configs[config];

	rb_init(&channel->rb_rx, channel->buffer_rx, BUF_SIZE);
	rb_init(&channel->rb_tx, channel->buffer_tx, BUF_SIZE);

	gpio_af_init(UART_TX_PORT[config], UART_TX_PIN[config], GPIO_HIGH_SPEED, GPIO_PUSHPULL, UART_AF[config]);
	gpio_af_init(UART_RX_PORT[config], UART_RX_PIN[config], GPIO_HIGH_SPEED, GPIO_PUSHPULL, UART_AF[config]);
	
	USART_TypeDef* instance = UART_INSTANCE[config];

	if (instance == USART6) {
		instance_channels[0] = channel_configs[config];
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);

		NVIC_SetPriority(USART6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
		NVIC_EnableIRQ(USART6_IRQn);
	} else if (instance == USART3) {
		instance_channels[1] = channel_configs[config];
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);

		NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
		NVIC_EnableIRQ(USART3_IRQn);
		
	} else if (instance == USART2) {
		instance_channels[2] = channel_configs[config];
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);

		NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
		NVIC_EnableIRQ(USART2_IRQn);
	} else if (instance == USART1){
		instance_channels[3] = channel_configs[config];
		SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

		NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
		NVIC_EnableIRQ(USART1_IRQn);
	}
    
	SET_BIT(instance->CR1, USART_CR1_TE);
	SET_BIT(instance->CR1, USART_CR1_RE);

	if (instance == USART1 || instance == USART6) {
		// APB2
		// 43.375
		// 0000 0010 1011 0110 = 0x02b6
        instance->BRR = 0x02b6;
    }
    else {
		// APB1
		// 21.6875
		//0000 0001 0101 1011 = 0x015b
       instance->BRR = 0x015b;
    }

	SET_BIT(instance->CR1, USART_CR1_RXNEIE);
	SET_BIT(instance->CR1, USART_CR1_UE);
}

void USARTx_IRQHandler(uint8_t config, USART_TypeDef* instance) {
	uint32_t isrflags = READ_REG(instance->SR);

	uart_channel* channel = channels + instance_channels[config];

	if (isrflags & USART_SR_RXNE) {
		uint8_t data_in = (uint8_t)(instance->DR & (uint8_t)0x00FF);
		rb_push(&channel->rb_rx, data_in);
	}

	if (isrflags & USART_SR_TXE) {
		uint8_t data_out;
		if (rb_pop(&channel->rb_tx, &data_out)) {
			CLEAR_BIT(instance->CR1, USART_CR1_TXEIE);
		} else {
			instance->DR = data_out & (uint8_t)0x00FF;
		}
	}
}

void USART6_IRQHandler() {
	USARTx_IRQHandler(instance_channels[0], USART6);
}

void USART3_IRQHandler() {
	USARTx_IRQHandler(instance_channels[1], USART3);
}

void USART2_IRQHandler() {
	USARTx_IRQHandler(instance_channels[2], USART2);
}

void USART1_IRQHandler() {
	USARTx_IRQHandler(instance_channels[3], USART1);
}

uint8_t uart_read_byte(uint8_t config) {
	uint8_t data;
	uart_channel* channel = channels + channel_configs[config];
	while (rb_pop(&channel->rb_rx, &data));
	return data;
}

void uart_send_byte(uint8_t config, uint8_t data) {
	uart_channel* channel = channels + channel_configs[config];
	USART_TypeDef* instance = UART_INSTANCE[config];

	while (rb_isFull(&channel->rb_tx));

	rb_push(&channel->rb_tx, data);
	SET_BIT(instance->CR1, USART_CR1_TXEIE);
}

void uart_send_buf(uint8_t config, uint8_t *data, size_t n) {
	for (size_t i = 0; i < n; i++) {
		uart_send_byte(config, data[i]);
	}
}
