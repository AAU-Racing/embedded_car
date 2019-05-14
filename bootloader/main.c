//#include <stdint.h>
//#include <stdbool.h>
//
//#include <board_driver/uart.h>
//#include <board_driver/flash.h>
//#include <board_driver/init.h>
//
//#define APPSIZE 256000
//#include "newlib_calls.h"
//
//int main(void) {
//	debug_uart_init(DEV_DEBUG_UART);
//
//	int counter = 0;
//	bool update = false;
//
//	while (1) {
//		uint8_t c = uart_read_byte(DEV_DEBUG_UART);
//		if (c == 'k') {
//			uart_send_buf(DEV_DEBUG_UART, (uint8_t[]){'y'}, 1);
//			break;
//		}
//
//		if(counter == 5) break;
//		counter++;
//	}
//
//	uint32_t start_address = 0x08010000;
//
//	for (size_t i = 0; i < len; i++) {
//		uint8_t c = uart_read_byte(DEV_DEBUG_UART);
//		data[i] = c;
//		uart_send_buf(DEV_DEBUG_UART, (uint8_t[]){c}, 1);
//	}
//
//	if (write_flash(start_address, data, len)) {
//		uart_send_buf(DEV_DEBUG_UART, (uint8_t[]){"Error\n"}, 6);
//		while(1);
//	} else {
//		uart_send_buf(DEV_DEBUG_UART, (uint8_t[]){"flash done\n"}, 11);
//	}
//
//	uart_send_buf(DEV_DEBUG_UART, (uint8_t[]){"Booting application...\n\n"}, 24);
//	boot(start_address);
//}
//

#include <stdint.h>
#include <stdbool.h>
#include <board_driver/usb/usb.h> 
#include <board_driver/flash.h>
#include <board_driver/init.h>
#include <bootloader/bootloader.h>

#include <board_driver/uart.h>
#include "../nodes/newlib_calls.h"

int main(void) {
	debug_uart_init(DEV_DEBUG_UART);

	//int len = 0, dataReceived = 0;
	//bool doUpdate = LookForUpdate();
	//
	//if(doUpdate) {
	//	Packet packet;
	//	RecievePacket(&packet);
	//	
	//	len = convert(packet.payload, packet.length);
	//	
	//	packet.id = 2;
	//	SendPacket(packet);
	//	
	//	while(dataReceived < len) {
	//		
	//	}
	//}

	//size_t len = 8132;
	//uint8_t data[len];

	//for (size_t i = 0; i < len; i++) {
	//	uint8_t c = uart_read_byte();
	//	data[i] = c;
	//	uart_send_buf((uint8_t[]){c}, 1);
	//}

	//if (write_flash(STARTADDRESS, data, len)) {
	//	uart_send_buf((uint8_t[]){"Error\n"}, 6);
	//	while(1);
	//} else {
	//	uart_send_buf((uint8_t[]){"flash done\n"}, 11);
	//}
	HAL_Delay(1000);
	printf("Booting application...\n\n");
	boot(STARTADDRESS);
}
