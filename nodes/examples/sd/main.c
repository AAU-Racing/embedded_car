#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <board_driver/uart.h>

#include <shield_drivers/com_node/sd.h>
#include <ff_gen_drv.h>
#include <sd_diskio.h>
#include <board_driver/rtc.h>

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
		create_file(file_name, "LOG", "CSV"); //Creates file and writes header
		printf("Successfully created file on SD\n");
	}

	appendData(file_name, "dataBuf", 7);
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
	uart_init();
	printf("UART init complete\n");

	//(void)BSP_RTC_Init();
	FIL f;
	//uint16_t i = 0;

	printf("init: %d\n", init_fatfs(&fs));
	HAL_Delay(10);

	//Date_Time_t now;
	//RTC_Get_Date_Time(&now);
	//char time_str[27] = {'\0'};
	//sprintf(time_str, "time: %02d:%02d:%02d, %02d/%02d/%02d\n", now.hours, now.minutes, now.seconds, now.date, now.month, (now.year + 1900));

	printf("open: %d\n", f_open(&f, "LOGFILE", FA_READ));
	HAL_Delay(10);

	{
		unsigned char text[32] = {0};
		unsigned int br = 0;
		int res = f_read(&f, &text, 5, &br);
		printf("br: %d\n", br);
		printf("res: %d\n", res);
		printf("text: %s\n", text);
		HAL_Delay(10);
	}

	printf("close: %d\n", f_close(&f));
	HAL_Delay(1000);

	printf("open: %d\n", f_open(&f, "LOGFILE", FA_WRITE));
	HAL_Delay(10);

	{
		unsigned char text[32] = "FUCK THIS";
		unsigned int bw = 0;
		int res = f_write(&f, &text, 5, &bw);
		printf("br: %d\n", bw);
		printf("res: %d\n", res);
		printf("text: %s\n", text);
		HAL_Delay(10);
	}

	printf("sync: %d\n", f_sync(&f));
	printf("close: %d\n", f_close(&f));
	HAL_Delay(1000);

	return 0;
}