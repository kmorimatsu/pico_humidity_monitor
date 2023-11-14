/*
	Define the flash area used for this counter.
	In default, use the last sector of 2 M flash area
	(FLASH_SECTOR_SIZE = 4096)
*/
#define FLASH_TARGET_OFFSET (2 * 1024 * 1024 - FLASH_SECTOR_SIZE)

/*
	Prototypes
*/
int read_flash_counter();
void reset_flash_counter();
void increment_flash_counter();
