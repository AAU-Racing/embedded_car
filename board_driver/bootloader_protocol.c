#include "board_driver/bootloader_protocol.h"
#include "board_driver/crc.h"
#include "device_driver/uart_driver.h"
#include <stdbool.h>
#include <string.h>

int (*receiveFunction)(Packet* packet);
int (*transmitFunction)(Packet packet);

void get_payload(Packet packet, uint8_t* arr, uint32_t* offset) {
	*offset += PAYLOADLENGTH;

	memcpy(arr + sizeof(uint8_t)*(*offset), packet.payload, PAYLOADLENGTH);
}

bool crc_is_valid(Packet packet) {
	return crc_calculate((uint32_t*)packet.payload, PAYLOADLENGTH) == packet.crc;
}

uint32_t get_image_length(Packet packet) {
	uint32_t i, imageLength = 0;

	for (i = 0; i < 4; i++)
	{
		imageLength = concatenate(imageLength, packet.payload[i]);
	}

	return imageLength;
}

uint32_t get_rtc_value(Packet packet) {
	uint32_t i, rtcValue = 0;

	for (i = 4; i < 8; i++)
	{
		rtcValue = concatenate(rtcValue, packet.payload[i]);
	}

	return rtcValue;
}

bool receive_packet(Packet* packet) {
	while(!receiveFunction(packet));

	if(crc_is_valid(*packet)) {
		return true;
	}
	else {
		Packet packet;
		create_packet(&packet, SET_UPDATE | SET_RETRANSMIT, NULL);
		transmit_packet(packet);

		return false;
	}
}

void transmit_packet(Packet packet) {
	transmitFunction(packet);
}

void create_packet(Packet* packet, uint8_t opId, uint8_t* payload) {
	packet->startId = 0xA1;
	packet->opId = opId;
	
	if(payload != NULL) {
		memcpy(packet->payload, payload, PAYLOADLENGTH);
		packet->crc = crc_calculate((uint32_t*)payload, PAYLOADLENGTH);
	}
}

uint32_t concatenate(uint32_t a, uint8_t b) {
	return (a << 8) | b;
}