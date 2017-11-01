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

#include <stdlib.h>
#include <stdio.h>
#include "LPC43xx.h"
#include "boot_interface.h"

#define BOOT_DATA       (struct boot_data_struct *)0x10080000
#define DATA_SOURCE     (unsigned char *)0x10080400

/*----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) 
{
	struct boot_data_struct *boot_data = BOOT_DATA;
	unsigned char *source = DATA_SOURCE;
	int state = 0;
	
	SystemCoreClockUpdate();
	
	state = boot_op(source, boot_data);
	
	return state;
}
