#include <stm32f4xx_hal.h>
#include <stdbool.h>
#include <string.h>

#include "i2c.h"

#define BUFFER_SIZE 256

static I2C_HandleTypeDef i2cHandle;

static void set_SDA_as_input_pin(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pin       = DASHBOARD_I2C_SDA_PIN;
	// GPIO_InitStruct.Alternate = 0; // Disablw, how?

	HAL_GPIO_Init(DASHBOARD_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void set_SCL_as_output_pin(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = DASHBOARD_I2C_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	HAL_GPIO_Init(DASHBOARD_I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

}

static bool SDA_is_low(void) {
	return HAL_GPIO_ReadPin(DASHBOARD_I2C_SDA_GPIO_PORT, DASHBOARD_I2C_SDA_PIN) == GPIO_PIN_RESET;
}

static void set_SCL_high(void) {
	HAL_GPIO_WritePin(DASHBOARD_I2C_SCL_GPIO_PORT, DASHBOARD_I2C_SCL_PIN, GPIO_PIN_SET);
}

static void toggle_SCL(void) {
	HAL_GPIO_TogglePin(DASHBOARD_I2C_SCL_GPIO_PORT, DASHBOARD_I2C_SCL_PIN);
}

static int bus_recovering(void) {
	set_SDA_as_input_pin();
	if (SDA_is_low()) {
		set_SCL_as_output_pin();
		set_SCL_high();

		int retry_limit = 100;
		while (SDA_is_low()) {
			toggle_SCL();
			if (!retry_limit--) return -1; // Should not take more than 8 cycles or there is another problem
		}
	}
	return 0;
}

int i2c_init(void) {
	bus_recovering();

	i2cHandle.Instance             = DASHBOARD_I2C;

	i2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT,//I2C_ADDRESSINGMODE_10BIT;
	i2cHandle.Init.ClockSpeed      = 400000;
	i2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9, //I2C_DUTYCYCLE_2;
	i2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	if(HAL_I2C_Init(&i2cHandle) != HAL_OK) {
		return -1;
	}

	return 0;
}

int i2c_is_ready(uint16_t addr) {
	return HAL_I2C_IsDeviceReady(&i2cHandle, addr, 1000, 1000);
}

int i2c_master_transmit(uint16_t addr, void *buf, size_t n) {

	if (HAL_I2C_Master_Transmit(&i2cHandle, addr, (uint8_t*)buf, n, 2) != HAL_OK) {

		return 1;
	}
	return 0;
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	DASHBOARD_I2C_SCL_GPIO_CLK_ENABLE();
	DASHBOARD_I2C_SDA_GPIO_CLK_ENABLE();
	/* Enable I2C1 clock */
	DASHBOARD_I2C_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* I2C TX GPIO pin configuration  */
	GPIO_InitStruct.Pin       = DASHBOARD_I2C_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = DASHBOARD_I2C_SCL_AF;

	HAL_GPIO_Init(DASHBOARD_I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

	/* I2C RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = DASHBOARD_I2C_SDA_PIN;
	GPIO_InitStruct.Alternate = DASHBOARD_I2C_SDA_AF;

	HAL_GPIO_Init(DASHBOARD_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
	(void) hi2c;

	/*##-1- Reset peripherals ##################################################*/
	DASHBOARD_I2C_FORCE_RESET();
	DASHBOARD_I2C_RELEASE_RESET();

	/*##-2- Disable peripherals and GPIO Clocks ################################*/
	/* Configure I2C Tx as alternate function  */
	HAL_GPIO_DeInit(DASHBOARD_I2C_SCL_GPIO_PORT, DASHBOARD_I2C_SCL_PIN);
	/* Configure I2C Rx as alternate function  */
	HAL_GPIO_DeInit(DASHBOARD_I2C_SDA_GPIO_PORT, DASHBOARD_I2C_SDA_PIN);
}
