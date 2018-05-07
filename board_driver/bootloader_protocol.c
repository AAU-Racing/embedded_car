#include "board_driver/bootloader_protocol.h"
#include "board_driver/crc.h"
#include <stdbool.h>
#include <string.h>

void GetPayload(Packet packet, uint8_t* arr, uint8_t* offset) {
	*offset += PAYLOADLENGTH;

	memcpy(arr + sizeof(uint8_t)*(*offset), packet.payload, PAYLOADLENGTH);
}

bool CRCIsValid(Packet packet) {
	return crc_calculate((uint32_t*)packet.payload, PAYLOADLENGTH) == packet.crc;
}

uint32_t GetImageLength(Packet packet) {
	uint32_t i, imageLength = 0;

	for (i = 0; i < 4; i++)
	{
		imageLength = Concatenate(imageLength, packet.payload[i]);
	}

	return imageLength;
}

uint32_t GetRTCValue(Packet packet) {
	uint32_t i, rtcValue = 0;

	for (i = 4; i < 8; i++)
	{
		rtcValue = Concatenate(rtcValue, packet.payload[i]);
	}

	return rtcValue;
}

Packet ReceivePacket(void) {
	Packet packet;

	return packet;
}

void TransmitPacket(Packet packet) {
	
}

void CreatePacket(Packet* packet, uint8_t opId, uint8_t* payload) {
	packet->startId = 0xA1;
	packet->opId = opId;
	
	if(payload != NULL) {
		memcpy(packet->payload, payload, PAYLOADLENGTH);
		packet->crc = crc_calculate((uint32_t*)payload, PAYLOADLENGTH);
	}
}

uint32_t Concatenate(uint32_t a, uint8_t b) {
	return (a << 8) | b;
}