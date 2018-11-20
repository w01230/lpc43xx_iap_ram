/* *
 *  boot interface
 *
 *
 *  get command from swd and excute the code
 */
 
#include "LPC43xx.h" 
#include "flash.h"
#include "eeprom.h"
#include "otp.h"
#include "boot_op.h"

int boot_op(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long offset = boot_data->command & 0x000F;
	unsigned long magic = boot_data->command & 0xF000;
	unsigned long device = boot_data->command & 0x0F00;
	struct boot_handler mem_handler[3] = {flash_handler, eeprom_handler, otp_handler};
	int (*handler_a)(void *) = (void *)0;
	int (*handler_b)(void *, void *) = (void *)0;
	
	state = flash_handler.init();
	if (state != BOOT_SUCCESS)
		return BOOT_ERROR;
	
	state = eeprom_handler.init();
	if (state != BOOT_SUCCESS)
		return BOOT_ERROR;
	
	if ((device >> 8) > 0x02)
		return BOOT_ERROR;
	
	if (offset > OP_END)
		return BOOT_ERROR;
	
	switch (magic) {
	case OPS_MAGIC_A:
		handler_a = (int (*) (void *))(*((unsigned long *)&mem_handler[device >> 8] + offset));
		state = handler_a(boot_data);
		break;
	
	case OPS_MAGIC_B:
		handler_b = (int (*) (void *, void *))(*((unsigned long *)&mem_handler[device >> 8] + offset));
		state = handler_b(source, boot_data);
		break;
	
	default:
		state = BOOT_ERROR;
		break;
	}
	
	return state;
}
