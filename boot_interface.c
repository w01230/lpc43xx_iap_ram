/* *
 *  boot interface
 *
 *
 *  get command from swd and excute the code
 */
 
#include "LPC43xx.h" 
#include "iap_interface.h"
#include "eeprom_interface.h"
#include "boot_interface.h"

int boot_op(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long offset = boot_data->command & 0x000F;
	unsigned long magic = boot_data->command & 0xF000;
	unsigned long part = boot_data->command & 0x0F00;
	int (*handler_a)(void *) = (void *)0;
	int (*handler_b)(void *, void *) = (void *)0;
	
	state = flash_handler.init();
	if (state != BOOT_SUCCESS)
		return BOOT_ERROR;
	
	state = eeprom_handler.init();
	if (state != BOOT_SUCCESS)
		return BOOT_ERROR;
	
	switch (magic) {
	case OPS_MAGIC_A:
		if (part == 0x0000)
			handler_a = (int (*) (void *))(*((unsigned long *)&flash_handler + offset)) ;
		else
			handler_a = (int (*) (void *))(*((unsigned long *)&eeprom_handler + offset));
		state = handler_a(boot_data);
		break;
	
	case OPS_MAGIC_B:
		if (part == 0x0000)
			handler_b = (int (*) (void *, void *))(*((unsigned long *)&flash_handler + offset));
		else
			handler_b = (int (*) (void *, void *))(*((unsigned long *)&eeprom_handler + offset));;
		state = handler_b(source, boot_data);
		break;
	
	default:
		break;
	}
	
	return state;
}
