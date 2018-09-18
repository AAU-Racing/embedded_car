#ifndef SD_H
#define SD_H

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_sd.h>

#define SD_CardInfo HAL_SD_CardInfoTypeDef

#define MSD_OK         0x00
#define MSD_ERROR      0x01

#define   SD_TRANSFER_OK                ((uint8_t)0x00)
#define   SD_TRANSFER_BUSY              ((uint8_t)0x01)

#define SD_DETECT_PIN                    GPIO_PIN_8
#define SD_DETECT_GPIO_PORT              GPIOA
#define __SD_DETECT_GPIO_CLK_ENABLE()    __GPIOD_CLK_ENABLE()
#define SD_DETECT_IRQn                   EXTI15_10_IRQn

#define SD_DATATIMEOUT           ((uint32_t)100000000)

#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)

/* DMA definitions for SD DMA transfer */
#define __DMAx_TxRx_CLK_ENABLE            __DMA2_CLK_ENABLE
#define SD_DMAx_Tx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Rx_CHANNEL                DMA_CHANNEL_4
#define SD_DMAx_Tx_STREAM                 DMA2_Stream6
#define SD_DMAx_Rx_STREAM                 DMA2_Stream3
#define SD_DMAx_Tx_IRQn                   DMA2_Stream6_IRQn
#define SD_DMAx_Rx_IRQn                   DMA2_Stream3_IRQn
#define SD_DMAx_Tx_IRQHandler             DMA2_Stream6_IRQHandler
#define SD_DMAx_Rx_IRQHandler             DMA2_Stream3_IRQHandler
#define SD_DetectIRQHandler()             HAL_GPIO_EXTI_IRQHandler(SD_DETECT_PIN)


uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
// uint8_t BSP_SD_ReadBlocks_DMA(uint8_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks);
// uint8_t BSP_SD_WriteBlocks_DMA(uint8_t *pData, uint64_t WriteAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr);
void    BSP_SD_IRQHandler(void);
// void    SD_DMA_Tx_IRQHandler(void);
// void    SD_DMA_Rx_IRQHandler(void);
uint8_t BSP_SD_GetCardState(void);
void    BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
uint8_t BSP_SD_IsDetected(void);

#endif /* SD_H */
