/**
 * boot interface header
 *
 *
 * get command from swd
 */
 
#ifndef _BOOT_H
#define _BOOT_H

#define OPS_MAGIC_A       (0xA000)
#define OPS_MAGIC_B       (0xB000)

enum boot_ops {
	IDCHECK = 0xA001,
	BLANKCHECK = 0xA002,
	ERASE = 0xA003,
	PROGRAM = 0xB004,
	VERIFY = 0xB005,
	SECURE = 0xA006,
	READ = 0xB007,
	OP_END= 0xA008,
};

enum boot_part {
	FLASH = 0x0000,
	EEPROM = 0x0100,
	OTP = 0x0200,
};

enum boot_state {
	BOOT_SUCCESS = 0x00,
	BOOT_ERROR = 0x20,
	BOOT_UNKNOWN = 0x21
};

struct boot_data_struct {
	enum boot_ops command;
	unsigned long parameter[5];
	unsigned long state;
	unsigned long result[5];
};

struct boot_handler {
	int (*init)(void);
	int (*id_check)(struct boot_data_struct *boot_data);
	int (*blank_check)(struct boot_data_struct *boot_data);
	int (*erase)(struct boot_data_struct *boot_data);
	int (*program)(unsigned char *source, struct boot_data_struct *boot_data);
	int (*compare)(unsigned char *source, struct boot_data_struct *boot_data);
	int (*secure)(struct boot_data_struct *boot_data);
	int (*read)(unsigned char *destination, struct boot_data_struct *boot_data);
};

int boot_op(unsigned char *source, struct boot_data_struct *boot_data);

#endif
