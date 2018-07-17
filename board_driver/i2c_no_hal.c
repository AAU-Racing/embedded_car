#include <stdbool.h>
#include <string.h>

#include "can.h"
#include "i2c_no_hal.h"
#include "gpio.h"

#define BUFFER_SIZE 256
#define WRITE 0

typedef struct {
	uint16_t addr;
	uint8_t buf[32];
	size_t n;
} i2c_msg;

static I2C_TypeDef *handle;

static void init_sda_pin() {
	gpio_af_init(I2C_SDA_GPIO_PORT, I2C_SDA_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, I2C_SDA_AF);
}

static void init_scl_pin() {
	gpio_af_init(I2C_SCL_GPIO_PORT, I2C_SCL_PIN, GPIO_HIGH_SPEED, GPIO_PUSHPULL, I2C_SCL_AF);
}

static void i2c_enable() {
	SET_BIT(handle->CR1, I2C_CR1_PE);
}

static void i2c_freqrange(){
	handle->CR2 = 40; // 40 MHz
}

static void i2c_rise_time(){
	// Sm mode bus allows a maximum rise time of 1000 ns
	// T_pclk1 = 1 / 40 MHz = 25 ns
	// TRISE = (1000 ns / T_pclk1) + 1 = 41
	handle->TRISE = 41;
}

static void i2c_speed(){
	// F_scl = 100 kHz
	// T_scl = 1 / F_scl
	// T_high = T_scl / 2 (because duty cycle is 1:1 and T_scl = T_high + T_low) = 5000 ns
	// T_pclk1 = 1 / 40 MHz = 25 ns
	// CCR = T_high / T_pclk1 = 200
	handle->CCR = 200;
}

static void i2c_start_clock(){
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);
}

int i2c_init(void) {
	//bus_recovering();
    init_sda_pin();
    init_scl_pin();

	handle = I2C2;
    i2c_start_clock();
	i2c_freqrange();
	i2c_speed();
	i2c_rise_time();
	i2c_enable();

	return 0;
}

int i2c_is_ready(uint16_t addr) {
	return 1;
}

static void start_condition(){
	printf("start\n");
	printf("CR1 %08x\n", handle->CR1);
	SET_BIT(handle->CR1, I2C_CR1_START);
	printf("start 2\n");
	// Clear EV5 by reading SR1 register
	handle->SR1;
	printf("start 3\n");
}

static void set_slave_addr(uint8_t addr) {
	// Shift for correct placement
	addr = (addr & 0x8f) << 1;

	// Set slave address
	handle->DR = addr | WRITE;

	// Clear ADDR flag by reading
	handle->SR1;
	handle->SR2;
}

static void wait_for_dr_empty(){
	//I2C_FLAG_TXE: Data register empty flag (1 means empty)
	while(READ_BIT(handle->SR1, I2C_SR1_TXE) == RESET) {}
}

static void transmit_byte(uint8_t byte) { // Write data to DR
	handle->DR = byte;
}

static void wait_for_byte_transfer_finished() {
	//I2C_FLAG_BTF: Byte transfer finished flag (1 means finished)
	while(READ_BIT(handle->SR1, I2C_SR1_BTF) == RESET) {}
}

static void stop_condition(){
	handle->CR1 |= I2C_CR1_STOP;
}

int i2c_master_transmit(uint16_t addr, uint8_t *buf, size_t n) { // No DMA
	printf("1\n");
	HAL_Delay(100);
	printf("2\n");
	start_condition();
	printf("3\n");
	set_slave_addr(addr);
	printf("4\n");

	for (uint8_t i = 0; i < n; i++) {
		wait_for_dr_empty();
		transmit_byte(buf[i]);
	}
	printf("5\n");
	wait_for_dr_empty();
	printf("6\n");
	wait_for_byte_transfer_finished();
	printf("7\n");
	stop_condition();
	printf("8\n");

	return 1;
}



/// ********** ///
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
			if (!retry_limit--)
				return -1; // Should not take more than 8 cycles or there is another problem
		}
        deinit_sda_as_gpio_input();
	}
    deinit_sda_as_gpio_input();
	return 0;
}
