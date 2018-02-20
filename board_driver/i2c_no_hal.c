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

static void init_sda_pin() {
	gpio_af_init(I2C_SDA_PORT, I2C_SDA_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, I2C_SDA_AF);
}

static void init_scl_pin() {
	gpio_af_init(I2C_SCL_PORT, I2C_SCL_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, I2C_SCL_AF);
}

static void init_sda_as_gpio_input() {

}

static void deinit_sda_as_gpio_input() {

}

static void init_scl_as_gpio_output() {

}

static bool sda_is_low() {
	return false;
}

static void set_scl_high() {
}

static void toogle_scl() {
}

static int bus_recovering(void) {
	init_sda_as_gpio_input();
	if (sda_is_low()) {
		init_scl_as_gpio_output();
		set_scl_high();

		int retry_limit = 100;
		while (sda_is_low()) {
			toogle_scl();
			if (!retry_limit--) return -1; // Should not take more than 8 cycles or there is another problem
		}
        deinit_sda_as_gpio_input();
	}
    deinit_sda_as_gpio_input();
	return 0;
}

int i2c_init(void) {
	//bus_recovering();
    init_sda_pin();
    init_scl_pin();

	handle = I2C;

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
