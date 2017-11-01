/*----------------------------------------------------------------------------
 * Name:    Blinky.c
 * Purpose: Dual Core LED Flasher for MCB4300
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2004-2014 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "nxp_iap.h"

#define BOOT_DATA        (struct boot_struct *)0x10080000
#define DATA_SOURCE     (unsigned char *)0x10080400

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
	struct boot_struct *boot_data = BOOT_DATA;
	unsigned int part_id = 0;
	unsigned long start = 0;
	unsigned long end = 0;
	unsigned long dest = 0;
	unsigned char *source = 0;
	unsigned long bank = 0;
	unsigned long cpu_khz = 0;
	unsigned long size = 0;
	int state = 0;
	
	SystemCoreClockUpdate ();
	cpu_khz = SystemCoreClock / 1024;
	
	while (1) {
		/* key should be send when all data is ready */
		if (boot_data->keyring != KEY_START)
			continue;
		
		switch(boot_data->command) {
		case IDCHECK:
			part_id = iap_read_part_id();
			if (part_id != (boot_data->parameter[1] << 16 | boot_data->parameter[0]))
				boot_data->state = IAP_ERROR;
			else
				boot_data->state = CMD_SUCCESS;
		break;
		
		case BLANKCHECK:
			start = boot_data->parameter[0];
			end = boot_data->parameter[1];
			bank = boot_data->parameter[2];
			state = iap_blankcheck_sector(start, end, bank);
			boot_data->state = state;
		break;
			
		case ERASE:
			start = boot_data->parameter[0];
			end = boot_data->parameter[1];
			bank = boot_data->parameter[2];
			state |= iap_prepare_for_write(start, end, bank);
			state = iap_erase_sector(start, end, cpu_khz, cpu_khz);
			boot_data->state = state;
		break;
		
		case PROGRAM:
			dest = boot_data->parameter[0];
			source = DATA_SOURCE;
			size = boot_data->parameter[1];
			
			start = boot_data->parameter[2];
			end = boot_data->parameter[3];
			bank = boot_data->parameter[4];
		
			state |= iap_prepare_for_write(start, end, bank);
			state = iap_copy_ram_to_flash(dest, source, size, cpu_khz);
			boot_data->state = state;
		break;
		
		case VERIFY:
			dest = boot_data->parameter[0];
			source = DATA_SOURCE;
			size = boot_data->parameter[1];
			state = iap_compare(dest, source, size);
			boot_data->state = state;
		break;
		
		case SECURE:
			bank = boot_data->parameter[0];
			state = iap_set_active_boot_bank(bank, cpu_khz);
			boot_data->state = state;
		break;
		
		default:
			break;
		}
		boot_data->keyring = 0xFFFFFFFF;
	}
}
