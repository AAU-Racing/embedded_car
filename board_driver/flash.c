#include <stm32f4xx_hal.h>

#include <stdint.h>
#include <stddef.h>

#include "flash.h"


int write_flash(uint32_t start_address, uint8_t *data, size_t len) {
	//unlock flash writing
	HAL_FLASH_Unlock();

	//clear flags.
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	//Erase sector.
	HandleSectorErase(len);

	for(size_t i = 0; i < len; i++) {
		if (HAL_FLASH_Program(TYPEPROGRAM_BYTE, start_address++, data[i]) != HAL_OK) {
			HAL_FLASH_Lock();
			return 1;
		}
	}

	HAL_FLASH_Lock();

	return 0;
}

void HandleSectorErase(size_t len) {
	if(len > SECTOR4SIZE) {
		FLASH_Erase_Sector(FLASH_SECTOR_4, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE + SECTOR7SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6 | FLASH_SECTOR_7, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE + SECTOR7SIZE + SECTOR8SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6 | FLASH_SECTOR_7 | FLASH_SECTOR_8, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE + SECTOR7SIZE + SECTOR8SIZE + SECTOR9SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6 | FLASH_SECTOR_7 | FLASH_SECTOR_8 | FLASH_SECTOR_9, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE + SECTOR7SIZE + SECTOR8SIZE + SECTOR9SIZE + SECTOR10SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6 | FLASH_SECTOR_7 | FLASH_SECTOR_8 | FLASH_SECTOR_9 | FLASH_SECTOR_10, FLASH_VOLTAGE_RANGE);
	}
	else if(len > (SECTOR4SIZE + SECTOR5SIZE + SECTOR6SIZE + SECTOR7SIZE + SECTOR8SIZE + SECTOR9SIZE + SECTOR10SIZE + SECTOR11SIZE)) {
		FLASH_Erase_Sector(FLASH_SECTOR_4 | FLASH_SECTOR_5 | FLASH_SECTOR_6 | FLASH_SECTOR_7 | FLASH_SECTOR_8 | FLASH_SECTOR_9 | FLASH_SECTOR_10 | FLASH_SECTOR_11, FLASH_VOLTAGE_RANGE);
	}
}