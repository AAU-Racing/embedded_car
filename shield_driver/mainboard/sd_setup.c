#include "sd_setup.h"


static SD_HandleTypeDef uSdHandle;

static void BSP_SD_MspInit(void);

static void init_detect_pin() {

	GPIO_InitTypeDef GPIO_Init_Structure;

	__SD_DETECT_GPIO_CLK_ENABLE();

	/* SD Card detect pin configuration */
	GPIO_Init_Structure.Mode      = GPIO_MODE_INPUT;
	GPIO_Init_Structure.Pull      = GPIO_PULLUP;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Pin       = SD_DETECT_PIN;
	HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_Init_Structure);
}

uint8_t BSP_SD_Init(void) {
	uint8_t SD_state = MSD_OK;

	/* uSD device interface configuration */
	uSdHandle.Instance = SDIO;

	uSdHandle.Init.ClockEdge           = SDIO_CLOCK_EDGE_RISING;
	uSdHandle.Init.ClockBypass         = SDIO_CLOCK_BYPASS_DISABLE;
	uSdHandle.Init.ClockPowerSave      = SDIO_CLOCK_POWER_SAVE_DISABLE;
	uSdHandle.Init.BusWide             = SDIO_BUS_WIDE_4B;
	uSdHandle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	uSdHandle.Init.ClockDiv            = SDIO_TRANSFER_CLK_DIV;

	init_detect_pin();

	/* Check if the SD card is plugged in the slot */
	if(BSP_SD_IsDetected() != SD_PRESENT) { // TODO: Says not present even though present
		return MSD_ERROR;
	}

	/* HAL SD initialization */
	BSP_SD_MspInit();
	if(HAL_SD_Init(&uSdHandle) != MSD_OK) {
		SD_state = MSD_ERROR;
	}

	/* Configure SD Bus width */
	if (SD_state == MSD_OK) {
	/* Enable wide operation */
		uint8_t result = HAL_SD_ConfigWideBusOperation(&uSdHandle, SDIO_BUS_WIDE_4B);
		printf("Status: %d\n", result);
		if (result != MSD_OK) {
			SD_state = MSD_ERROR;
		} else {
			SD_state = MSD_OK;
		}
	}

	return  SD_state;
}


// uint8_t SD_ITConfig(void) {
// 	GPIO_InitTypeDef GPIO_Init_Structure;
//
// 	/* Configure Interrupt mode for SD detection pin */
// 	GPIO_Init_Structure.Mode      = GPIO_MODE_IT_RISING_FALLING;
// 	GPIO_Init_Structure.Pull      = GPIO_PULLUP;
// 	GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
// 	GPIO_Init_Structure.Pin       = SD_DETECT_PIN;
// 	HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &GPIO_Init_Structure);
//
// 	/* NVIC configuration for SDIO interrupts */
// 	HAL_NVIC_SetPriority(SD_DETECT_IRQn, 5, 0);
// 	HAL_NVIC_EnableIRQ(SD_DETECT_IRQn);
//
// 	return 0;
// }


uint8_t BSP_SD_IsDetected(void) {
	return (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) != GPIO_PIN_RESET)
		? SD_NOT_PRESENT : SD_PRESENT;
}


uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout) {
	if (HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK) {
		return MSD_ERROR;
	}
	else {
		return MSD_OK;
	}
}


uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint64_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout) {
	if (HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK) {
        return MSD_ERROR;
    }
    else {
        return MSD_OK;
    }
}

// uint8_t SD_ReadBlocks_DMA(uint8_t *pData, uint64_t ReadAddr, uint32_t NumOfBlocks) {
// 	uint8_t SD_state = MSD_OK;
//
// 	/* Read block(s) in DMA transfer mode */
// 	if(HAL_SD_ReadBlocks_DMA(&uSdHandle, pData, ReadAddr, NumOfBlocks) != MSD_OK) {
// 		SD_state = MSD_ERROR;
// 	}
//
// 	/* Wait until transfer is complete */
// 	if(SD_state == MSD_OK) {
// 		while (HAL_SD_GetState(&uSdHandle) != HAL_SD_STATE_READY) { }
// 	}
//
// 	return SD_state;
// }
//
//
// uint8_t SD_WriteBlocks_DMA(uint8_t *pData, uint64_t WriteAddr, uint32_t NumOfBlocks) {
// 	uint8_t SD_state = MSD_OK;
//
// 	/* Write block(s) in DMA transfer mode */
// 	if(HAL_SD_WriteBlocks_DMA(&uSdHandle, pData, WriteAddr, NumOfBlocks) != MSD_OK) {
// 		SD_state = MSD_ERROR;
// 	}
//
// 	/* Wait until transfer is complete */
// 	if(SD_state == MSD_OK) {
// 		while (HAL_SD_GetState(&uSdHandle) != HAL_SD_STATE_READY) { }
// 	}
//
// 	return SD_state;
// }

// uint8_t BSP_SD_Erase(uint64_t StartAddr, uint64_t EndAddr) {
// 	if (HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != MSD_OK) {
// 		return MSD_ERROR;
// 	} else {
// 		return MSD_OK;
// 	}
// }

static void BSP_SD_MspInit(void) {
	// static DMA_HandleTypeDef dmaRxHandle;
	// static DMA_HandleTypeDef dmaTxHandle;
	GPIO_InitTypeDef GPIO_Init_Structure;
	// SD_HandleTypeDef *hsd = &uSdHandle;

	/* Enable SDIO clock */
	__SDIO_CLK_ENABLE();

	/* Enable DMA2 clocks */
	// __DMAx_TxRx_CLK_ENABLE();

	/* Enable GPIOs clock */
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();

	/* Common GPIO configuration */
	GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
	GPIO_Init_Structure.Pull      = GPIO_PULLUP;
	GPIO_Init_Structure.Speed     = GPIO_SPEED_HIGH;
	GPIO_Init_Structure.Alternate = GPIO_AF12_SDIO;

	/* GPIOC configuration */
	GPIO_Init_Structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;

	HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

	/* GPIOD configuration */
	GPIO_Init_Structure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

	/* NVIC configuration for SDIO interrupts */
	HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(SDIO_IRQn);

	/* Configure DMA Rx parameters */
	// dmaRxHandle.Init.Channel             = SD_DMAx_Rx_CHANNEL;
	// dmaRxHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	// dmaRxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	// dmaRxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	// dmaRxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	// dmaRxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	// dmaRxHandle.Init.Mode                = DMA_PFCTRL;
	// dmaRxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	// dmaRxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	// dmaRxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	// dmaRxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	// dmaRxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
	//
	// dmaRxHandle.Instance = SD_DMAx_Rx_STREAM;
	//
	// /* Associate the DMA handle */
	// __HAL_LINKDMA(hsd, hdmarx, dmaRxHandle);
	//
	// /* Deinitialize the stream for new transfer */
	// HAL_DMA_DeInit(&dmaRxHandle);
	//
	// /* Configure the DMA stream */
	// HAL_DMA_Init(&dmaRxHandle);
	//
	// /* Configure DMA Tx parameters */
	// dmaTxHandle.Init.Channel             = SD_DMAx_Tx_CHANNEL;
	// dmaTxHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	// dmaTxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
	// dmaTxHandle.Init.MemInc              = DMA_MINC_ENABLE;
	// dmaTxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	// dmaTxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
	// dmaTxHandle.Init.Mode                = DMA_PFCTRL;
	// dmaTxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
	// dmaTxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
	// dmaTxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
	// dmaTxHandle.Init.MemBurst            = DMA_MBURST_INC4;
	// dmaTxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
	//
	// dmaTxHandle.Instance = SD_DMAx_Tx_STREAM;
	//
	// /* Associate the DMA handle */
	// __HAL_LINKDMA(hsd, hdmatx, dmaTxHandle);
	//
	// /* Deinitialize the stream for new transfer */
	// HAL_DMA_DeInit(&dmaTxHandle);
	//
	// /* Configure the DMA stream */
	// HAL_DMA_Init(&dmaTxHandle);
	//
	// /* NVIC configuration for DMA transfer complete interrupt */
	// HAL_NVIC_SetPriority(SD_DMAx_Rx_IRQn, 6, 0);
	// HAL_NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);
	//
	// /* NVIC configuration for DMA transfer complete interrupt */
	// HAL_NVIC_SetPriority(SD_DMAx_Tx_IRQn, 6, 0);
	// HAL_NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}


void SD_IRQHandler(void) {
	HAL_SD_IRQHandler(&uSdHandle);
}


// void SD_DMA_Tx_IRQHandler(void) {
// 	HAL_DMA_IRQHandler(uSdHandle.hdmatx);
// }
//
//
// void SD_DMA_Rx_IRQHandler(void) {
// 	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
// }

uint8_t BSP_SD_GetCardState(void) {
    return ((HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}


void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo) {
	HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}
