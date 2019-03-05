#include <stdio.h>

#include "gpio.h"
#include "uart.h"
#include "ringbuffer.h"

#define BUF_SIZE 1024
#define CHANNELS 3
#define CONFIGS 7

typedef struct {
	ringbuffer_t rb_tx;
	ringbuffer_t rb_rx;
	uint8_t buffer_tx[BUF_SIZE];
	uint8_t buffer_rx[BUF_SIZE];
} uart_channel;

typedef struct {
	uint8_t channel;
	GPIO_Pin tx_pin;
	GPIO_Pin rx_pin;
	GPIO_TypeDef *tx_port;
	GPIO_TypeDef *rx_port;
	GPIO_AlternateFunction tx_af;
	GPIO_AlternateFunction rx_af;
	
} uart_config;

uart_channel channels[CHANNELS];

uint8_t channel_configs[CONFIGS] = { 255, 255, 255, 255, 255, 255, 255 };

ringbuffer_t uartx_rec;
ringbuffer_t uartx_send;
uint8_t uartx_rec_buf[BUF_SIZE];
uint8_t uartx_send_buf[BUF_SIZE];


void uart_init(uart_config *config) {

	uart_channel channel = channels[config->channel];

	rb_init(&channel.rb_rx, channel.buffer_rx, BUF_SIZE);
	rb_init(&channel.rb_tx, channel.buffer_tx, BUF_SIZE);


	gpio_af_init(config->tx_port, config->tx_pin, GPIO_HIGH_SPEED, GPIO_PUSHPULL, config->tx_af);
	gpio_af_init(config->rx_port, config->rx_pin, GPIO_HIGH_SPEED, GPIO_PUSHPULL, config->rx_af);

    SET_BIT(USARTx_CLKR, USARTx_CLK_Msk);

    NVIC_SetPriority(USARTx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
    NVIC_EnableIRQ(USARTx_IRQn);

	SET_BIT(USARTx->CR1, USART_CR1_TE);
	SET_BIT(USARTx->CR1, USART_CR1_RE);

	if (USARTx == USART1 || USARTx == USART6) {
		// APB2
		// 43.375
		// 0000 0010 1011 0110 = 0x02b6
        USARTx->BRR = 0x02b6;
    }
    else {
		// APB1
		// 21.6875
		//0000 0001 0101 1011 = 0x015b
       USARTx->BRR = 0x015b;
    }

	SET_BIT(USARTx->CR1, USART_CR1_RXNEIE);
	SET_BIT(USARTx->CR1, USART_CR1_UE);
}

void USART3_IRQHandler(void) {
	USARTx_IRQHandler(2, USART3);
}

void USARTx_IRQHandler(uint8_t config, USART_HandleTypeDef* instance) {
	uint32_t isrflags = READ_REG(instance->SR);

	if (isrflags & USART_SR_RXNE) {
		uint8_t data_in = (uint8_t)(USARTx->DR & (uint8_t)0x00FF);
		rb_push(&uartx_rec, data_in);
	}

	if (isrflags & USART_SR_TXE) {
		uint8_t data_out;
		if (rb_pop(&uartx_send, &data_out)) {
			CLEAR_BIT(USARTx->CR1, USART_CR1_TXEIE);
		} else {
			USARTx->DR = data_out & (uint8_t)0x00FF;
		}
	}
}

uint8_t uart_read_byte(void) {
	uint8_t data;
	while (rb_pop(&uartx_rec, &data));
	return data;
}

void uart_send_byte(uint8_t data) {
	while (rb_isFull(&uartx_send));
	rb_push(&uartx_send, data);
	SET_BIT(USARTx->CR1, USART_CR1_TXEIE);
}

void uart_send_buf(uint8_t *data, size_t n) {
	for (size_t i = 0; i < n; i++) {
		uart_send_byte(data[i]);
	}
}
