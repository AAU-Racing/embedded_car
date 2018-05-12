#include "bootloader.h"
#include "stddef.h"

void StartUpdate(Packet* packet, uint32_t* len, uint32_t* rtc) {
	while(1) {
		receive_packet(packet);

		if(IS_UPDATE(packet->startId) && IS_PREPARE_UPDATE(packet->opId)) {
			*len = get_image_length(*packet);
			*rtc = get_rtc_value(*packet);

			break;
		}
	}
}

void ReceiveImage(Packet* packet, uint8_t* data, uint32_t* offset) {
	while(1) {
		if(receive_packet(packet)) {

			if(IS_UPDATE(packet->startId) && IS_TRANSFER_BEGIN(packet->opId)) {
				get_payload(*packet, data, offset);
			}

			if(IS_UPDATE(packet->startId) && IS_TRANSFER_CONTINUE(packet->opId)) {
				get_payload(*packet, data, offset);
				create_packet(packet, SET_TRANSFER_CONTINUE | SET_UPDATE, NULL);
			}

			if(IS_UPDATE(packet->startId) && IS_TRANSFER_END(packet->opId)) {
				get_payload(*packet, data, offset);

				break;
			}
		}
	}
}

void UpdateImage(uint32_t address, Packet* packet, uint8_t* data, uint32_t len) {
	if (write_flash(address, data, len)) {
		create_packet(packet, SET_STATUS, (uint8_t[]){"Error\n"});
		transmit_packet(*packet);
		while(1);
	} else {
		create_packet(packet, SET_STATUS, (uint8_t[]){"flash done\n"});
		transmit_packet(*packet);
	}
}