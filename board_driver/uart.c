#include <stm32f4xx_hal.h>
#include <stdio.h>

#include "gpio.h"
#include "uart.h"
#include "ringbuffer.h"

#define BUF_SIZE 1024

UART_HandleTypeDef UartHandle;
ringbuffer_t uartx_rec;
ringbuffer_t uartx_send;
uint8_t uartx_rec_buf[BUF_SIZE];
uint8_t uartx_send_buf[BUF_SIZE];

void uart_init(void) {
	UartHandle.Instance = USARTx;

	gpio_af_init(USARTx_TX_GPIO_PORT, USARTx_TX_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, USARTx_TX_AF);
	gpio_af_init(USARTx_RX_GPIO_PORT, USARTx_RX_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, USARTx_RX_AF);

    SET_BIT(USARTx_CLKR, USARTx_CLK_Msk);

    NVIC_SetPriority(USARTx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0x1, 0x0));
    NVIC_EnableIRQ(USARTx_IRQn);

	SET_BIT(USARTx->CR1, USART_CR1_TE);
	SET_BIT(USARTx->CR1, USART_CR1_RE);

	if (USARTx == USART1 || USARTx == USART6)
    {
      USARTx->BRR =  (((UART_DIVMANT_SAMPLING16((HAL_RCC_GetPCLK2Freq), (_BAUD_)) << 4U) + \
                                                        (UART_DIVFRAQ_SAMPLING16((HAL_RCC_GetPCLK2Freq), (_BAUD_)) & 0xF0U)) + \
                                                        (UART_DIVFRAQ_SAMPLING16((HAL_RCC_GetPCLK2Freq), (_BAUD_)) & 0x0FU))

    }
    else
    {
      USARTx->BRR = UART_BRR_SAMPLING16(HAL_RCC_GetPCLK1Freq(), huart->Init.BaudRate);
    }

	UartHandle.Init.BaudRate     = 115200;
	UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits     = UART_STOPBITS_1;
	UartHandle.Init.Parity       = UART_PARITY_NONE;
	UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode         = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	rb_init(&uartx_rec, uartx_rec_buf, BUF_SIZE);
	rb_init(&uartx_send, uartx_send_buf, BUF_SIZE);
	HAL_UART_Init(&UartHandle);
	SET_BIT(USARTx->CR1, USART_CR1_RXNEIE);
}

void USARTx_IRQHandler(void) {
	uint32_t isrflags = READ_REG(UartHandle.Instance->SR);

	if (isrflags & USART_SR_RXNE) {
		uint8_t data_in = (uint8_t)(UartHandle.Instance->DR & (uint8_t)0x00FF);
		rb_push(&uartx_rec, data_in);
	}

	if (isrflags & USART_SR_TXE) {
		uint8_t data_out;
		if (rb_pop(&uartx_send, &data_out)) {
			CLEAR_BIT(UartHandle.Instance->CR1, USART_CR1_TXEIE);
		} else {
			UartHandle.Instance->DR = data_out & (uint8_t)0x00FF;
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
	SET_BIT(UartHandle.Instance->CR1, USART_CR1_TXEIE);
}

void uart_send_buf(uint8_t *data, size_t n) {
	for (size_t i = 0; i < n; i++) {
		uart_send_byte(data[i]);
	}
}
