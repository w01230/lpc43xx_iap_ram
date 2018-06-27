/**
 * OTP bank interface
 *
 *
 * Program OTP bank
 */
 
/**
 * otp_init
 *
 *
 * init otp
 */
#include <string.h>
#include "boot_op.h"
#include "otp.h"

static inline unsigned long *get_otp_api(unsigned long offset)
{
	return *((unsigned long **)(*(unsigned long *)OTP_API_BASE + offset));
}

static unsigned long otp_int(void)
{
	unsigned long state = 0;
	unsigned long (*handler) (void) = (unsigned long (*)(void))get_otp_api(OTP_INIT);
	
	state = handler();
	
	return state;
}

/**
 * check_bits
 *
 *
 * check wich bit to program and set it to '1'
 */
static unsigned long check_bits(unsigned long *source, struct boot_data_struct *boot_data)
{
	unsigned long state = 0;
	unsigned long otp = 0x00;
	unsigned long data = 0x00;
	
//	for (int i = 0; i < OTP_BANK_SIZE * 3; i++) {
//		otp = *(unsigned long *)(OTP_DATA_BASE + i * sizeof(unsigned long));
//		//data = *(source + i);
//		*(source + i) = otp;
//	}
//	*(source + 3) = 0x01028044;
	
	for (int i = 0; i < OTP_BANK_SIZE * 3; i++) {
		otp = *(unsigned long *)(OTP_DATA_BASE + i * sizeof(unsigned long));
		data = *(source + i);
		*(source + i) = otp ^ data;
	}
	
	return state;
}
/**
 * otp_prog_bootsrc
 *
 *
 * prog boot src bits
 */

static unsigned long otp_program_bootsrc(enum otp_bootsrc bootsrc)
{
	unsigned long state = 0;
	unsigned long (*handler) (enum otp_bootsrc) = (void *) 0;
	
	handler = (unsigned long (*) (enum otp_bootsrc bootsrc))get_otp_api(OTP_PROG_BOOTSRC);
	
	state = handler(bootsrc);
	if (state == OTP_ALL_DATAORMASK_ZERO)
		state = 0;
	
	return state;
}

/**
 * otp_program_usbid
 *
 *
 * program usb id
 */

static unsigned long otp_program_usbid(unsigned long product_id, unsigned long vendor_id)
{
	unsigned long state = 0;
	unsigned long (*handler) (unsigned long, unsigned long) = (void *) 0;
	
	handler = (unsigned long (*) (unsigned long, unsigned long))get_otp_api(OTP_PROG_USBID);
	state = handler(product_id, vendor_id);
	if (state == OTP_ALL_DATAORMASK_ZERO)
		state = 0;;
	
	return state;
}

/**
 * otp_program_gpx
 *
 *
 * program gp0~2
 */
static unsigned long otp_program_gpx(enum otp_api_offset gpx, unsigned long *data, unsigned long *mask)
{
	unsigned long state = 0;
	unsigned long (*handler) (unsigned long *, unsigned long *) = (void *) 0;
	volatile unsigned long gp0 = 0;
	
	if ((gpx < OTP_PROG_GP0) || (gpx > OTP_PROG_GP2))
		return OTP_PROGRAM_ERROR;
	
	handler = (unsigned long (*) (unsigned long *, unsigned long *))get_otp_api(gpx);
	state = handler(data, mask);
	if (state == OTP_ALL_DATAORMASK_ZERO)
		state = 0;
	
	return state;
}

/**
 * otp_program_gpx
 *
 *
 * program gp0~2
 */
static unsigned long otp_program_gp2x(enum otp_api_offset gpx, unsigned long data, unsigned long mask)
{
	unsigned long state = 0;
	unsigned long (*handler) (unsigned long , unsigned long) = (void *) 0;
	volatile unsigned long gp0 = 0;
	
	if ((gpx < OTP_PROG_GP20) || (gpx > OTP_PROG_GP22))
		return OTP_PROGRAM_ERROR;
	
	handler = (unsigned long (*) (unsigned long, unsigned long))get_otp_api(gpx);
	state = handler(data, mask);
	if (state == OTP_ALL_DATAORMASK_ZERO)
		state = 0;
	
	return state;
}

/**
 * otp_program
 *
 *
 * program gp0~2 / boot src
 */
static int otp_program(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long *data = (unsigned long *)(source);
	int gp2usb = boot_data->parameter[0];
	enum otp_bootsrc bootsrc = BOOTSRC_PINS;
	unsigned long usb_data[2] = {*(data + 9) >> 16, (*(data + 9) >> 16) * 0xFFFF};
	unsigned long *gp_data[3] = {data + 0x00, data + 0x04, data + 0x08};
	
	do {
		state = otp_int();
		if (state != 0)
			break;
				
		state = check_bits(data, boot_data);
		if (state != 0)
			break;
		
		bootsrc = (enum otp_bootsrc)((*(data + 8) >> 25) & 0x0F);
		state = otp_program_bootsrc(bootsrc);
		if (state != 0)
			break;
		
		if (gp2usb != 0)
			state = otp_program_usbid(usb_data[0], usb_data[1]);
		else
			state = otp_program_gp2x(OTP_PROG_GP20, *(gp_data[2] + 1), *(gp_data[2] + 1));
		if (state != 0)
			break;
		
		state = otp_program_gpx(OTP_PROG_GP0, gp_data[0], gp_data[0]);
		if (state != 0)
			break;
		
		state = otp_program_gpx(OTP_PROG_GP1, gp_data[1], gp_data[1]);
		if (state != 0)
			break;
		
		state = otp_program_gp2x(OTP_PROG_GP21, *(gp_data[2] + 2), *(gp_data[2] + 2));
		if (state != 0)
			break;
		
		state = otp_program_gp2x(OTP_PROG_GP22, *(gp_data[2] + 3), *(gp_data[2] + 3));
		if (state != 0)
			break;
	} while(0);
	
	return state;
}

static int otp_compare(unsigned char *source, struct boot_data_struct *boot_data)
{
	int state = 0;
	unsigned long otp = 0x00;
	unsigned long data = 0x00;
	unsigned long offset = 0;
	
	/* compare gp0 and gp1 */
	for (int i = 0; i < OTP_BANK_SIZE * 2; i++) {
		otp = *(unsigned long *)(OTP_DATA_BASE + i * sizeof(unsigned long));
		data = *(unsigned long *)(source + i * sizeof(unsigned long));
		if (otp != data)
			return OTP_COMPARE_ERROR;
	}
	
	/* compare gp2 word 0 */
	offset = OTP_BANK_SIZE * 2 * sizeof(unsigned long);
	otp = *(unsigned long *)(OTP_DATA_BASE + offset);
	data = *(unsigned long *)(source + offset);
	if (((data >> 25) & 0x0F) != ((otp >> 25) & 0x0F))
		return OTP_COMPARE_ERROR;
	
	/* compare gp20 and gp22 */
	offset = OTP_BANK_SIZE * 2 * sizeof(unsigned long) + 4;
	for (int i = 0; i < OTP_BANK_SIZE - 1; i++) {
		otp = *(unsigned long *)(OTP_DATA_BASE + offset + i * sizeof(unsigned long));
		data = *(unsigned long *)(source + offset + i * sizeof(unsigned long));
		if (otp != data)
			return OTP_COMPARE_ERROR;
	}
	
	return state;
}

struct boot_handler otp_handler = {
	.init = (int (*)(void))otp_int,
	.id_check = (void *)0,
	.blank_check = (void *)0,
	.erase = (void *)0,
	.program = otp_program,
	.compare = otp_compare,
	.secure = (void *)0,
	.read = (void *)0,
};
