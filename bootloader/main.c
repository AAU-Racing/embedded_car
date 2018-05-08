#include <stdint.h>
#include <stdbool.h>

#include <board_driver/uart.h>
#include <board_driver/usb/usb.h>
#include <board_driver/can.h>
#include <board_driver/flash.h>
#include <board_driver/init.h>
#include <board_driver/bootloader_protocol.h>
#include <board_driver/rtc.h>

#define STARTADDRESS 0x08010000

int main(void) {
	uart_init();

	//TODO Find a way to receive packets with generic interface.

	uint8_t offset = 0;
	uint32_t len = 0, rtc = 0;

	while(1) {
		Packet startPacket;
		receive_packet(&startPacket);

		if(IS_UPDATE(startPacket.startId) && IS_PREPARE_UPDATE(startPacket.opId)) {
			len = get_image_length(startPacket);
			rtc = get_rtc_value(startPacket);

			break;
		}
	}

	uint8_t data[len];

	RTC_Update_Date_Time(rtc);

	while(1) {
		Packet packet;
		receive_packet(&packet);

		if(IS_UPDATE(packet.startId) && IS_TRANSFER_BEGIN(packet.opId)) {
			get_payload(packet, data, &offset);
		}

		if(IS_UPDATE(packet.startId) && IS_TRANSFER_CONTINUE(packet.opId)) {
			get_payload(packet, data, &offset);
		}

		if(IS_UPDATE(packet.startId) && IS_TRANSFER_END(packet.opId)) {
			get_payload(packet, data, &offset);

			break;
		}

		if(!crc_is_valid(packet)) {
			create_packet(&packet, SET_UPDATE | SET_RETRANSMIT, NULL);
			transmit_packet(packet);
		}
	}

	Packet packet;

	if (write_flash(STARTADDRESS, data, len)) {
		create_packet(&packet, SET_STATUS, (uint8_t[]){"Error\n"});
		transmit_packet(packet);
		while(1);
	} else {
		create_packet(&packet, SET_STATUS, (uint8_t[]){"flash done\n"});
		transmit_packet(packet);
	}
	
	create_packet(&packet, SET_STATUS, (uint8_t[]){"Booting application...\n\n"});
	transmit_packet(packet);
	
	boot(STARTADDRESS);
}
