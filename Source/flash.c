/* *
 *  IAP interface for LPC4357
 *
 *
 *  allow write data to interal flash
 */

#include "LPC43xx.h"
#include "crc32.h"
#include "boot_op.h"
#include "flash.h"

/**
 * iap init
 *
 *
 * initialize iap programming
 */
 
static int iap_init(void)
{
	struct iap_struct iap_data;
	
	iap_data.command = INIT;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * prepare for write
 *
 *
 * prepare for write operation
 */
 
static int iap_prepare_for_write(unsigned long start, unsigned long end, unsigned long bank)
{
	struct iap_struct iap_data;
	
	iap_data.command = PREPARE_FOR_WRITE;
	iap_data.parameter[0] = start;
	iap_data.parameter[1] = end;
	iap_data.parameter[2] = bank;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * copy ram to flash
 *
 *
 * copy data from ram to flash
 */
 
static int iap_copy_ram_to_flash(unsigned long destination, unsigned char *source, unsigned long length, unsigned long cpu_khz)
{
	struct iap_struct iap_data;
	
	iap_data.command = COPY_RAM_TO_FLASH;
	iap_data.parameter[0] = destination;
	iap_data.parameter[1] = (unsigned long)source;
	iap_data.parameter[2] = length;
	iap_data.parameter[3] = cpu_khz;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * erase sectors
 *
 *
 * copy data from ram to flash
 */
 
static int iap_erase_sector(unsigned long start, unsigned long end, unsigned long cpu_khz, unsigned long bank)
{
	struct iap_struct iap_data;
	
	iap_data.command = ERASE_SECTORS;
	iap_data.parameter[0] = start;
	iap_data.parameter[1] = end;
	iap_data.parameter[2] = cpu_khz;
	iap_data.parameter[3] = bank;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

///**
// * erase page
// *
// *
// * copy data from ram to flash
// */
// 
//static int iap_erase_page(unsigned long start, unsigned long end, unsigned long cpu_khz)
//{
//	struct iap_struct iap_data;
//	
//	iap_data.command = ERASE_PAGE;
//	iap_data.parameter[0] = start,
//	iap_data.parameter[1] = end,
//	iap_data.parameter[2] = cpu_khz,
//	iap_operation(&iap_data.command, &iap_data.state);
//	
//	if (iap_data.state != CMD_SUCCESS)
//		return iap_data.state;
//	
//	return 0;
//}

/**
 * erase sectors
 *
 *
 * copy data from ram to flash
 */
 
static int iap_blankcheck_sector(unsigned long start, unsigned long end, unsigned long bank)
{
	struct iap_struct iap_data;
	
	iap_data.command = BLANK_CHECK_SECTORS;
	iap_data.parameter[0] = start;
	iap_data.parameter[1] = end;
	iap_data.parameter[2] = bank;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * read part identification
 *
 *
 * read id of the part
 */
 
unsigned static int iap_read_part_id(void)
{
	struct iap_struct iap_data;
	
	iap_data.command = READ_ID;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return iap_data.result[0];
}

/**
 * compare 
 *
 *
 * compare data in different address
 */
 
static int iap_compare(unsigned long destination, unsigned char *source, unsigned long length)
{
	struct iap_struct iap_data;
	
	iap_data.command = COMPARE;
	iap_data.parameter[0] = destination;
	iap_data.parameter[1] = (unsigned long)source;
	iap_data.parameter[2] = length;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * set active boot flash bank 
 *
 *
 * set which flash bank is active for boot
 */
 
static int iap_set_active_boot_bank(unsigned long bank, unsigned cpu_khz)
{
	struct iap_struct iap_data;
	
	iap_data.command = SET_ACTIVE_BOOT_BANK;
	iap_data.parameter[0] = bank;
	iap_data.parameter[1] = cpu_khz;
	iap_operation(&iap_data.command, &iap_data.state);
	
	if (iap_data.state != CMD_SUCCESS)
		return iap_data.state;
	
	return 0;
}

/**
 * boot_idcheck
 *
 *
 * init iap and eeprom
 */

static int flash_idcheck(struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long part_id = 0x00;
	
	part_id = iap_read_part_id();
	if (part_id != boot_data->parameter[0])
		return BOOT_ERROR;
	
	boot_data->result[0] = part_id;
	
	return state;
}

/**
 * boot_blankcheck
 *
 *
 * init iap and eeprom
 */

static int flash_blankcheck(struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long start = 0x00;
	unsigned long end = 0x00;
	unsigned long bank = 0x00;
	
	start = boot_data->parameter[0];
	end = boot_data->parameter[1];
	bank = boot_data->parameter[2];
	state = iap_blankcheck_sector(start, end, bank);
	
	return state;
}

/**
 * boot_erase
 *
 *
 * init iap and eeprom
 */

static int flash_erase(struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long start = 0x00;
	unsigned long end = 0x00;
	unsigned long bank = 0x00;
	unsigned long cpu_khz = SystemCoreClock / 1000;
	
	start = boot_data->parameter[0];
	end = boot_data->parameter[1];
	bank = boot_data->parameter[2];
	state = iap_prepare_for_write(start, end, bank);
	state |= iap_erase_sector(start, end, cpu_khz, bank);
	
	return state;
}

/**
 * boot_program
 *
 *
 * init iap and eeprom
 */

static int flash_program(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long dest = 0x00;
	unsigned long size = 0x00;
	unsigned long start = 0x00;
	unsigned long end = 0;
	unsigned long bank = 0;
	unsigned long cpu_khz = SystemCoreClock / 1000;
	unsigned long crc32 = 0;
	
	dest = boot_data->parameter[0];
	size = boot_data->parameter[1];

	start = boot_data->parameter[2];
	end = boot_data->parameter[3];
	bank = boot_data->parameter[4];

	crc32 = cacuate_crc32(source, size, 0);
	if (crc32 != *(unsigned long *)(source + size)) {
		boot_data->state = BOOT_ERROR;
		boot_data->result[0] = crc32;
		boot_data->result[1] = *(unsigned long *)(source + size);
		return BOOT_ERROR;
	}
	
	state = iap_prepare_for_write(start, end, bank);
	state |= iap_copy_ram_to_flash(dest, source, size, cpu_khz);
	
	return state;
}

/**
 * boot_verify
 *
 *
 * init iap and eeprom
 */

static int flash_compare(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long dest = 0x00;
	unsigned long size = 0x00;
	unsigned long crc32 = 0;
	
	dest = boot_data->parameter[0];
	size = boot_data->parameter[1];
	crc32 = cacuate_crc32(source, size, 0);
	if (crc32 != *(unsigned long *)(source + size)) {
		boot_data->state = BOOT_ERROR;
		boot_data->result[0] = crc32;
		boot_data->result[1] = *(unsigned long *)(source + size);
		return BOOT_ERROR;
	}
	state |= iap_compare(dest, source, size);
	
	return state;
}

/**
 * boot_secure
 *
 *
 * init iap and eeprom
 */

static int flash_secure(struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long bank = boot_data->parameter[0];
	unsigned long cpu_khz = SystemCoreClock / 1000;
	
	state = iap_set_active_boot_bank(bank, cpu_khz);
	
	return state;
}

struct boot_handler flash_handler = {
	.init = iap_init,
	.id_check = flash_idcheck,
	.blank_check = flash_blankcheck,
	.erase = flash_erase,
	.program = flash_program,
	.compare = flash_compare,
	.secure = flash_secure,
	.read = (void *)0,
};
