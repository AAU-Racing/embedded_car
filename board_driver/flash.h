#include <stdint.h>
#include <stddef.h>

#define SECTOR4SIZE  (1024 * 64)
#define SECTOR5SIZE  (1024 * 128)
#define SECTOR6SIZE  (1024 * 128)
#define SECTOR7SIZE  (1024 * 128)
#define SECTOR8SIZE  (1024 * 128)
#define SECTOR9SIZE  (1024 * 128)
#define SECTOR10SIZE (1024 * 128)
#define SECTOR11SIZE (1024 * 128)

#define FLASH_VOLTAGE_RANGE VOLTAGE_RANGE_3

int write_flash(uint32_t start_address, uint8_t *data, size_t len);
void HandleSectorErase(size_t len);
