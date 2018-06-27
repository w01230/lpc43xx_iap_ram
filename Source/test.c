/*
 * test code
 *
 * test otp
 */
 
 /*
 * @brief LPC18xx/43xx OTP Controller driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define BOOTROM_BASE            0x10400100
#define OTP_API_TABLE_OFFSET    0x1

static unsigned long *BOOTROM_API_TABLE;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
extern unsigned long Chip_OTP_Init(void);
/*****************************************************************************
 * Private functions
 ****************************************************************************/
/**
 * @brief	OTP Boot Source selection used in Chip driver
 */
typedef enum CHIP_OTP_BOOT_SRC {
	CHIP_OTP_BOOTSRC_PINS,		/*!< Boot source - External pins */
	CHIP_OTP_BOOTSRC_UART0,		/*!< Boot source - UART0 */
	CHIP_OTP_BOOTSRC_SPIFI,		/*!< Boot source - EMC 8-bit memory */
	CHIP_OTP_BOOTSRC_EMC8,		/*!< Boot source - EMC 16-bit memory */
	CHIP_OTP_BOOTSRC_EMC16,		/*!< Boot source - EMC 32-bit memory */
	CHIP_OTP_BOOTSRC_EMC32,		/*!< Boot source - EMC 32-bit memory */
	CHIP_OTP_BOOTSRC_USB0,		/*!< Boot source - DFU USB0 boot */
	CHIP_OTP_BOOTSRC_USB1,		/*!< Boot source - DFU USB1 boot */
	CHIP_OTP_BOOTSRC_SPI,		/*!< Boot source - SPI boot */
	CHIP_OTP_BOOTSRC_UART3		/*!< Boot source - UART3 */
} CHIP_OTP_BOOT_SRC_T;

static unsigned long (*Otp_ProgBootSrc)(CHIP_OTP_BOOT_SRC_T BootSrc);
static unsigned long (*Otp_ProgJTAGDis)(void);
static unsigned long (*Otp_ProgUSBID)(unsigned long ProductID, unsigned long VendorID);
static unsigned long (*Otp_ProgGP0)(unsigned long Data, unsigned long Mask);
static unsigned long (*Otp_ProgGP1)(unsigned long Data, unsigned long Mask);
static unsigned long (*Otp_ProgGP2)(unsigned long Data, unsigned long Mask);
static unsigned long (*Otp_ProgKey1)(unsigned char *key);
static unsigned long (*Otp_ProgKey2)(unsigned char *key);
static unsigned long (*Otp_GenRand)(void);

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* CHIP OTP Initialisation function */
unsigned long Chip_OTP_Init(void)
{
	unsigned long (*ROM_otp_Init)(void);

	BOOTROM_API_TABLE = *((unsigned long * *) BOOTROM_BASE + OTP_API_TABLE_OFFSET);

	ROM_otp_Init      = (unsigned long (*)(void))BOOTROM_API_TABLE[0];
	Otp_ProgBootSrc   = (unsigned long (*)(CHIP_OTP_BOOT_SRC_T BootSrc))BOOTROM_API_TABLE[1];
	Otp_ProgJTAGDis   = (unsigned long (*)(void))BOOTROM_API_TABLE[2];
	Otp_ProgUSBID     = (unsigned long (*)(unsigned long ProductID, unsigned long VendorID))BOOTROM_API_TABLE[3];
	Otp_ProgGP0       = (unsigned long (*)(unsigned long Data, unsigned long Mask))BOOTROM_API_TABLE[8];
	Otp_ProgGP1       = (unsigned long (*)(unsigned long Data, unsigned long Mask))BOOTROM_API_TABLE[9];
	Otp_ProgGP2       = (unsigned long (*)(unsigned long Data, unsigned long Mask))BOOTROM_API_TABLE[10];
	Otp_ProgKey1      = (unsigned long (*)(unsigned char *key))BOOTROM_API_TABLE[11];
	Otp_ProgKey2      = (unsigned long (*)(unsigned char *key))BOOTROM_API_TABLE[12];
	Otp_GenRand       = (unsigned long (*)(void))BOOTROM_API_TABLE[13];

	return ROM_otp_Init();
}

/* Program boot source in OTP Controller */
unsigned long Chip_OTP_ProgBootSrc(CHIP_OTP_BOOT_SRC_T BootSrc)
{
	return Otp_ProgBootSrc(BootSrc);
}

/* Program the JTAG bit in OTP Controller */
unsigned long Chip_OTP_ProgJTAGDis(void)
{
	return Otp_ProgJTAGDis();
}

/* Program USB ID in OTP Controller */
unsigned long Chip_OTP_ProgUSBID(unsigned long ProductID, unsigned long VendorID)
{
	return Otp_ProgUSBID(ProductID, VendorID);
}

/* Program OTP GP Word memory */
unsigned long Chip_OTP_ProgGPWord(unsigned long WordNum, unsigned long Data, unsigned long Mask)
{
	unsigned long status;

	switch (WordNum) {
	case 1:
		status = Otp_ProgGP1(Data, Mask);
		break;

	case 2:
		status = Otp_ProgGP2(Data, Mask);
		break;

	case 0:
	default:
		status = Otp_ProgGP0(Data, Mask);
		break;
	}

	return status;
}

/* Program AES Key */
unsigned long Chip_OTP_ProgKey(unsigned long KeyNum, unsigned char *key)
{
	unsigned long status;

	if (KeyNum) {
		status = Otp_ProgKey2(key);
	}
	else {
		status = Otp_ProgKey1(key);
	}
	return status;
}

/* Generate Random Number using HW Random Number Generator */
unsigned long Chip_OTP_GenRand(void)
{
	return Otp_GenRand();
}






