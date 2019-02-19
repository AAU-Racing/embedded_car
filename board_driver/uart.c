#include <stm32f4xx_hal.h>
#include <stdio.h>

#include "gpio.h"
#include "uart.h"
#include "ringbuffer.h"

#define BUF_SIZE 1024

ringbuffer_t uartx_rec;
ringbuffer_t uartx_send;
uint8_t uartx_rec_buf[BUF_SIZE];
uint8_t uartx_send_buf[BUF_SIZE];

void uart_init(void) {
	rb_init(&uartx_rec, uartx_rec_buf, BUF_SIZE);
	rb_init(&uartx_send, uartx_send_buf, BUF_SIZE);

	gpio_af_init(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, USARTx_TX_AF);
	gpio_af_init(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, USARTx_RX_AF);

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

void USARTx_IRQHandler(void) {
	uint32_t isrflags = READ_REG(USARTx->SR);

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
