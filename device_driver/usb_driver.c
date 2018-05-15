#include "usb_driver.h"
#include "board_driver/bootloader_protocol.h"
#include "board_driver/usb/usb.h"

int usb_driver_receive_packet(Packet* packet) {
	if(usb_recieve() != 0xA1) {
		return 0;
	}
	
	int i = 0;
	
	uint32_t crc = 0;

	packet->startId = 0xA1;

	packet->opId = usb_recieve();

	for (i = 0; i < PAYLOADLENGTH; i++)
	{
		packet->payload[i] = usb_recieve();
	}

	for (i = 0; i < CRCLENGTH; i++)
	{
		crc = concatenate(crc, usb_recieve());
	}

	return 1;
}

void usb_driver_transmit_packet(Packet packet) {
	usb_send(packet.startId);

	usb_send(packet.opId);

	usb_send(packet.payload, PAYLOADLENGTH);

	usb_send((uint8_t*)packet.crc, CRCLENGTH);
}
