/* *
 *  eeprom operations for NXP LPC4357
 *
 *
 *  allow write data to eerom without wait vaule update.
 */

#include "LPC43xx.h"
#include "crc32.h"
#include "boot_op.h"
#include "eeprom.h"

/**
 * iap init
 *
 *
 * initialize eeprom
 */
 
static int eeprom_init(void)
{
	LPC_EEPROM->PWRDWN = EEPROM_NO_PWRDWN;
	LPC_EEPROM->AUTOPROG = EEPROM_AUTOPROG_ON;
	LPC_EEPROM->CLKDIV = SystemCoreClock / EEPROM_CLOCK_DIV - 1;
	LPC_EEPROM->INTENSET = EEPROM_INT_PROGSETEN;
	
	if ((LPC_EEPROM->AUTOPROG & EEPROM_AUTOPROG_ON) != EEPROM_AUTOPROG_ON)
		return -1;
	
	return 0;
}

/**
 * eeprom_write_page
 *
 *
 * write the eeprom one page
 */
 
static int eeprom_write_page(unsigned long *source, unsigned char page)
{
	int state = 0;
	unsigned long *offset = (unsigned long *) EEPROM_ADDRESS(page, 0);
	unsigned int timeout = SystemCoreClock;
	
	for (int i = 0; i < EEPROM_PAGE_SIZE / 4; i++)  {
		*(offset + i) = *(source + i);
	}
	
	while(timeout-- != 0) {
		if ((LPC_EEPROM->INTSTAT & EEPROM_INT_ENDOFPROG) == EEPROM_INT_ENDOFPROG)
			break;
		
		if (timeout == 1)
			return EEPROM_PROGRAM_ERROR;
	}
	LPC_EEPROM->INTSTATCLR = EEPROM_INT_PROGCLRST;
	
	return state;
}

/**
 * eeprom_compare_page
 *
 *
 * compare the eeprom one page
 */
 
static int eeprom_compare_page(unsigned long *source, unsigned char page)
{
	int state = 0;
	unsigned long *offset = (unsigned long *) EEPROM_ADDRESS(page, 0);
	
	for (int i = 0; i < EEPROM_PAGE_SIZE / 4; i++) {
		if (*(source + i) != *(offset + i)) {
			state = EEPROM_COMPARE_ERROR;
			break;
		}
	}
	
	return state;
}

/**
 * eeprom_read_page
 *
 *
 * read the eeprom one page
 */

static int eeprom_read_page(unsigned long *destination, unsigned char page)
{
	int state = 0;
	unsigned long *offset = (unsigned long *) EEPROM_ADDRESS(page, 0);
	
	for (int i = 0; i < EEPROM_PAGE_SIZE / 4; i++) {
		*(destination + i) = *(offset + i);
	}
	
	return state;
}

/**
 * eeprom_program
 *
 *
 * program the eeprom
 */

int eeprom_program(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long page = 0;
	unsigned long size = 0;
	unsigned int crc32 = 0;
	
	page = boot_data->parameter[0];
	size = boot_data->parameter[1];
	
	crc32 = cacuate_crc32(source, size, 0);
	if (crc32 != *(unsigned long *)(source + size)) {
		boot_data->state = BOOT_ERROR;
		boot_data->result[0] = crc32;
		boot_data->result[1] = *(unsigned long *)(source + size);
		return BOOT_ERROR;
	}
	
	state = eeprom_write_page((unsigned long *)source, page);
	
	return state;
}

/**
 * eeprom_compare
 *
 *
 * compare the eeprom
 */

int eeprom_compare(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long page = 0;
	unsigned long size = 0;
	unsigned int crc32 = 0;
	
	page = boot_data->parameter[0];
	size = boot_data->parameter[1];
	
	crc32 = cacuate_crc32(source, size, 0);
	if (crc32 != *(unsigned long *)(source + size)) {
		boot_data->state = BOOT_ERROR;
		boot_data->result[0] = crc32;
		boot_data->result[1] = *(unsigned long *)(source + size);
		return BOOT_ERROR;;
	}
	
	state = eeprom_compare_page((unsigned long *)source, page);
	
	return state;
}

/**
 * eeprom_compare
 *
 *
 * compare the eeprom
 */

int eeprom_read(unsigned char *destination, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long page = 0;
	
	page = boot_data->parameter[0];
	
	state = eeprom_read_page((unsigned long *)destination, page);
	
	return state;
}

struct boot_handler eeprom_handler = {
	.init = eeprom_init,
	.id_check = (void *)0,
	.blank_check = (void *)0,
	.erase = (void *)0,
	.program = eeprom_program,
	.compare = eeprom_compare,
	.secure = (void *)0,
	.read = eeprom_read,
};
