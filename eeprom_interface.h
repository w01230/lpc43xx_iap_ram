/* *
 *  eeprom operations for NXP LPC4357
 *
 *
 *  allow write data to eerom without wait vaule update.
 */
#ifndef __EEPROM_INTERFACE_H
#define __EEPROM_INTERFACE_H

#define EEPROM_CMD_ERASE_PRG_PAGE       (6)

#define EEPROM_START                    (0x20040000)
#define EEPROM_PAGE_SIZE                (128)

/* The number of EEPROM pages. The last page is not writable. */
#define EEPROM_PAGE_NUM                 (128)

/* Get the eeprom address */
#define EEPROM_ADDRESS(page, offset)    ((EEPROM_START) + (EEPROM_PAGE_SIZE * (page)) + (offset))
#define EEPROM_CLOCK_DIV                (1500000)

#define EEPROM_AUTOPROG_ON              (1 << 1)
#define EEPROM_NO_PWRDWN                (0 << 0)
#define EEPROM_INT_PROGSETEN            (1 << 2)
#define EEPROM_INT_ENDOFPROG            (1 << 2)
#define EEPROM_INT_PROGCLRST            (1 << 2)

enum eeprom_state {
	EEPROM_SUCCESS = 0x00,
	EEPROM_PROGRAM_ERROR = 0x20,
	EEPROM_COMPARE_ERROR = 0x21
};

extern struct boot_handler eeprom_handler;

#endif
