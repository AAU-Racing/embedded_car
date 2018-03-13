#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <board_driver/uart.h>

#include <shield_drivers/com_node/sd.h>
#include <ff_gen_drv.h>
#include <sd_diskio.h>

static uint8_t dataBuf[SD_STANDARD_BLOCK_SIZE] = {'\0'};
static uint8_t* BufEnd = dataBuf;
static char file_name[16] = {'\0'};
static FATFS fs;
static uint8_t count = 0;
static uint32_t last_write_time = 0;

void append_buffer_to_sd(void){
	uint16_t data_length = BufEnd - dataBuf;
	appendData(file_name, dataBuf, data_length);
	memset(dataBuf, '\0', SD_STANDARD_BLOCK_SIZE);
	BufEnd = dataBuf;
	last_write_time = HAL_GetTick();
}

void output_data(void *data, uint8_t size){
	for(uint8_t i = 0; i < size; i++){
		*BufEnd = *(uint8_t*)(data + i); //Transfering to main buffer
		//data[i] = '\0';
		BufEnd++;

		if(BufEnd >= (dataBuf + SD_STANDARD_BLOCK_SIZE)){
			append_buffer_to_sd();
		}
	}
}

static int init_fatfs(FATFS* fs) {
	char SDPath[4];

	// SD_Driver is exported from sd_diskio.h
	if (FATFS_LinkDriver(&SD_Driver, SDPath) != 0) return -1;
	if (f_mount(fs, SDPath, 1) != FR_OK) return -1;
	return 0;
}

void setup(void){
	uart_init();
	printf("UART init complete\n");

	(void)BSP_RTC_Init();

	if (init_fatfs(&fs) == 0) {
		create_file(file_name); //Creates file and writes header
		printf("Successfully created file on SD\n");
	}
}

void loop(void){
	output_data(&count, 1);
	count++;

	if(last_write_time + 1000 < HAL_GetTick()){ //If its been over a second since last write to SD
		append_buffer_to_sd();
	}

	HAL_Delay(50);
	printf("Writing to SD: %d\n", count);
}

int main(void){
	setup();

	while(1){
		loop();
	}

	return 0;
}