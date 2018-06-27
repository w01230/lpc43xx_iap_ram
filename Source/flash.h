/*
 * iap interface for LPC4357
 *
 *
 * Get data from ram
 */
#ifndef _FLASH_H
#define _FLASH_H

struct iap_struct {
	unsigned long command;
	unsigned long parameter[5];
	unsigned long state;
	unsigned long result[4];
};

typedef void (*iap_entry)(unsigned long *command, unsigned long *state);
#define iap_operation ((iap_entry) *((unsigned long *) 0x10400100))

enum iap_command {
	INIT = 49,
	PREPARE_FOR_WRITE = 50,
	COPY_RAM_TO_FLASH = 51,
	ERASE_SECTORS = 52,
	BLANK_CHECK_SECTORS = 53,
	READ_ID = 54,
	READ_BOOT_VERSION = 55,
	COMPARE = 56,
	REINVOKE_ISP = 57,
	ERASE_PAGE = 59,
	SET_ACTIVE_BOOT_BANK = 60
};

enum iap_state {
	CMD_SUCCESS = 0x0000,
	INVALID_COMMAND = 0x0001,
	SRC_ADDR_ERROR = 0x0002,
	DST_ADDR_ERROR = 0x0003,
	SRC_ADDR_NOT_MAPPED = 0x0004,
	DST_ADDR_NOT_MAPPED = 0x0005,
	COUNT_ERROR = 0x0006,
	INVALID_SECTOR = 0x0007,
	SECTOR_NOT_BLANK = 0x0008,
	SECTOR_NOT_MAPPED = 0x0009,
	COMPARE_ERROR = 0x000A,
	BUSY = 0x000B,
	PARAM_ERROR = 0x000C,
	ADDR_ERROR = 0x000D,
	ADDR_NOT_MAPPED = 0x000E,
	CMD_LOCKED = 0x000F,
	INVALID_CODE = 0x010,
	CODE_READ_PROTECTION_ENABLE = 0x0013,
	INVALID_FLASH_UNIT = 0x0014,
	USER_CODE_CHECKSUM = 0x0015,
	ERROR_SETTING_ACTIVE_PARTITION = 0x0016,
};

extern struct boot_handler flash_handler;

#endif
