#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/usb/usb.h>
#include <board_driver/can.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>
#include <board_driver/bootloader_protocol.h>
#include <board_driver/bootloader.h>
#include <board_driver/rtc.h>

#define STARTADDRESS 0x08010000

int main(void) {
	uart_init();

	//TODO Find a way to receive packets with generic interface.

	uint32_t len = 0, rtc = 0, offset = 0;

	Packet packet;

	StartUpdate(&packet, &len, &rtc);

	uint8_t data[len];

	RTC_Update_Date_Time(rtc);

	ReceiveImage(&packet, data, &offset);
	
	UpdateImage(STARTADDRESS, &packet, data, len);

	create_packet(&packet, SET_STATUS, (uint8_t[]){"Booting application...\n\n"});
	transmit_packet(packet);
	
	boot(STARTADDRESS);
}
