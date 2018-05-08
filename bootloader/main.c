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
		Packet startPacket = ReceivePacket();

		if(crc_is_valid(startPacket) && IS_UPDATE(startPacket.startId) && IS_PREPARE_UPDATE(startPacket.opId)) {
			len = GetImageLength(startPacket);
			rtc = GetRTCValue(startPacket);

			break;
		}
	}

	uint8_t data[len];

	RTC_Update_Date_Time(rtc);

	while(1) {
		Packet packet = ReceivePacket();

		if(crc_is_valid(packet) && IS_UPDATE(packet.startId) && IS_TRANSFER_BEGIN(packet.opId)) {
			GetPayload(packet, data, &offset);
		}

		if(crc_is_valid(packet) && IS_UPDATE(packet.startId) && IS_TRANSFER_CONTINUE(packet.opId)) {
			GetPayload(packet, data, &offset);
		}

		if(crc_is_valid(packet) && IS_UPDATE(packet.startId) && IS_TRANSFER_END(packet.opId)) {
			GetPayload(packet, data, &offset);

			break;
		}

		if(!crc_is_valid(packet)) {
			CreatePacket(&packet, SET_UPDATE | SET_RETRANSMIT, NULL);
			TransmitPacket(packet);
		}
	}

	Packet packet;

	if (write_flash(STARTADDRESS, data, len)) {
		CreatePacket(&packet, SET_STATUS, (uint8_t[]){"Error\n"});
		TransmitPacket(packet);
		while(1);
	} else {
		CreatePacket(&packet, SET_STATUS, (uint8_t[]){"flash done\n"});
		TransmitPacket(packet);
	}
	
	CreatePacket(&packet, SET_STATUS, (uint8_t[]){"Booting application...\n\n"});
	TransmitPacket(packet);
	
	boot(STARTADDRESS);
}
