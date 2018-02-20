#include <stdbool.h>
#include <string.h>

#include "can.h"
#include "i2c_no_hal.h"
#include "gpio.h"

#define BUFFER_SIZE 256

typedef struct {
	uint16_t addr;
	uint8_t buf[32];
	size_t n;
} i2c_msg;

static I2C_HandleTypeDef i2cHandle;
static I2C_TypeDef *handle;

static void set_SDA_as_input_pin(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Pin       = DASHBOARD_I2C_SDA_PIN;
	// GPIO_InitStruct.Alternate = 0; // Disablw, how?

	gpio_af_init(DASHBOARD_I2C_SDA_GPIO_PORT, &GPIO_InitStruct);
}

static void set_SCL_as_output_pin(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin       = DASHBOARD_I2C_SCL_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	gpio_af_init(DASHBOARD_I2C_SCL_GPIO_PORT, &GPIO_InitStruct);
}

static bool SDA_is_low(void) {
	return HAL_GPIO_ReadPin(DASHBOARD_I2C_SDA_GPIO_PORT, DASHBOARD_I2C_SDA_PIN) == GPIO_PIN_RESET;
}

static void set_SCL_high(void) {
}

static void toggle_SCL(void) {
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

	handle = DASHBOARD_I2C;

	i2c_freqrange();
	i2c_rise_time();
	i2c_speed();

	if(HAL_I2C_Init(&i2cHandle) != HAL_OK) {
		return -1;
	}

	return 0;
}

void i2c_freqrange(){
	handle->CR2 = 40; // 40 MHz
}

void i2c_rise_time(){
	handle->TRISE = ((((40) * 300U) / 1000U) + 1U)) // 40 is from freqrange
}

void i2c_speed(){ // pclk1 = (168/((40<<1U)))
	handle->CCR = 2.1
}

void i2c_cr1_con(){
	handle->I2C_CR1_NOSTRETCH = 0x00U; // Set bit 7 to 0
	handle->I2C_CR1_ENGC = 0x00U; // Set bit 6 to 0
}

void i2c_oar1_con(){
	handle->I2C_ADDRESSINGMODE_7BIT
}



int i2c_is_ready(uint16_t addr) {
}

int i2c_master_transmit_DMA(uint16_t addr, void *buf, size_t n) {
}

void I2C_MspInit(I2C_HandleTypeDef *hi2c) {

    DASHBOARD_I2C_SCL_GPIO_PORT();
    DASHBOARD_I2C_SDA_GPIO_CLK_ENABLE();

    DASHBOARD_I2C_CLK_ENABLE();
    DASHBOARD_I2C_DMA_CLK_ENABLE();

}
