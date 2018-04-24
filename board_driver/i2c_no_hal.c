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

    i2c_start_clock();
	i2c_freqrange();
	i2c_rise_time();
	i2c_speed();
	i2c_cr1_con();
	i2c_oar1_con();
	i2c_oar2_con();


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
	CLEAR_BIT(handle->CR1, I2C_CR1_NOSTRETCH); // Set bit 7 to 0
	CLEAR_BIT(handle->CR1, I2C_CR1_ENGC); // Set bit 6 to 0
	SET_BIT(handle->CR1, I2I2C_CR1_PE)
}

void i2c_oar1_con(){
    CLEAR_BIT(handle->OAR1, I2C_OAR1_ADDMODE); // Set bit 15 to 0
}

void i2c_oar2_con(){
	CLEAR_BIT(handle->OAR2, I2C_OAR2_ENDUAL); // Set bit 0 to 0
}

void i2c_start_clock(){
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);
}

int i2c_is_ready(uint16_t addr) {
}

int i2c_master_transmit(uint16_t addr, uint8_t *buf, size_t n) { // No DMA
	CLEAR_BIT(handle->CR1, I2C_CR1_POS);
	I2C_MasterRequestWrite(addr, *buf, n);

	return 1;
}

void I2C_MasterRequestWrite(uint16_t addr, uint8_t *buf, size_t n){
	start_condition();
	set_slave_addr();

	for (int i = 0; i < n; i++) {
		wait_for_dr_empty();
		transmit_byte(*(buf + i));
	}

	//I2C_FLAG_BTF: Byte transfer finished flag
	wait_for_byte_transfer_finished(addr, I2C_FLAG_BTF);
	stop_condition();
}

void start_condition(){
	SET_BIT(handle->CR1, I2C_CR1_START);

	while(!READ_BIT(handle->SR1, I2C_SR1_SB)) {	}
}

void set_slave_addr() {
	// Set slave address
	handle->DR = ((uint8_t)((addr) & (~I2C_OAR1_ADD0)));

	// Clear ADDR flag by reading
	handle->SR1;
	handle->SR2;
}

void wait_for_dr_empty() {
	//I2C_FLAG_TXE: Data register empty flag
	while(I2C_FLAG_TXE == FALSE){}
}

void transmit_byte(uint8_t byte, I2C_FLAG_BTF) { // Write data to DR
	handle->DR = byte; // hi2c->Instance->DR = (*hi2c->pBuffPtr++);
}

void wait_for_byte_transfer_finished(uint16_t addr, ){
	while(__HAL_I2C_GET_FLAG(addr, I2C_FLAG_BTF) == RESET){}
}

void stop_condition(){
	hi2c->Instance->CR1 |= I2C_CR1_STOP;
}
