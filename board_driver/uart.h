#ifndef UART_H
#define UART_H

#include <stm32f4xx_hal.h>
#include "gpio.h"

#define B1  0
#define C10 0
#define C6  1

#if B1

#define USARTx                           USART3
#define USARTx_CLKR                      RCC->APB1ENR
#define USARTx_CLK_Msk                   RCC_APB1ENR_USART3EN

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    PIN_10
#define USARTx_TX_GPIO_PORT              GPIOB
#define USARTx_TX_AF                     GPIO_AF7
#define USARTx_RX_PIN                    PIN_11
#define USARTx_RX_GPIO_PORT              GPIOB
#define USARTx_RX_AF                     GPIO_AF7

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART3_IRQn
#define USARTx_IRQHandler                USART3_IRQHandler

#elif C10

#define USARTx                           UART4

#define USARTx_CLKR                      RCC->APB1ENR
#define USARTx_CLK_Msk                   RCC_APB1ENR_UART4EN

/* Definition for UARTx Pins */
#define USARTx_TX_PIN                    PIN_10
#define USARTx_TX_GPIO_PORT              GPIOC
#define USARTx_TX_AF                     GPIO_AF8
#define USARTx_RX_PIN                    PIN_11
#define USARTx_RX_GPIO_PORT              GPIOC
#define USARTx_RX_AF                     GPIO_AF8

/* Definition for UARTx's NVIC */
#define USARTx_IRQn                      UART4_IRQn
#define USARTx_IRQHandler                UART4_IRQHandler

#elif C6

#define USARTx                           USART6

#define USARTx_CLKR                      RCC->APB2ENR
#define USARTx_CLK_Msk                   RCC_APB2ENR_USART6EN

/* Definition for UARTx Pins */
#define USARTx_TX_PIN                    PIN_6
#define USARTx_TX_GPIO_PORT              GPIOC
#define USARTx_TX_AF                     GPIO_AF8
#define USARTx_RX_PIN                    PIN_7
#define USARTx_RX_GPIO_PORT              GPIOC
#define USARTx_RX_AF                     GPIO_AF8

/* Definition for UARTx's NVIC */
#define USARTx_IRQn                      USART6_IRQn
#define USARTx_IRQHandler                USART6_IRQHandler

#endif


void uart_init(void);
uint8_t uart_read_byte(void);
void uart_send_byte(uint8_t data);
void uart_send_buf(uint8_t *data, size_t n);


#endif /* UART_H */
