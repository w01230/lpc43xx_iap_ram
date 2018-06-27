/**
 * OTP header file
 *
 *
 * header file for OTP
 */
 
#ifndef _OTP_H
#define _OTP_H

#define OTP_API_BASE       (0x10400104)
#define OTP_DATA_BASE      (0x40045010)        // skip the first bank because the first one is not writeable
#define OTP_BANK_SIZE      (4)                 // 4 word(cpu) 4x4=16 bytes
#define OTP_MASK_OFFSET    (0x0C)              // 0x0C(12) x 4 = 48 bytes

enum otp_bootsrc {
	BOOTSRC_PINS,
	BOOTSRC_UART0,
	BOOTSRC_SPIFI,
	BOOTSRC_EMMC8,
	BOOTSRC_EMMC16,
	BOOTSRC_USB0,
	BOOTSRC_USB1,
	BOOTSRC_UART3,
};

enum otp_api_offset {
	OTP_INIT = 0x00,
	OTP_PROG_BOOTSRC = 0x04,
	OTP_PROG_JTAG_DISABLE = 0x08,
	OTP_PROG_USBID = 0x0C,
	OTP_PROG_GP0 = 0x14,
	OTP_PROG_GP1 = 0x18,
	OTP_PROG_GP2 = 0x1C,
	OTP_PROG_GP20 = 0x20,
	OTP_PROG_GP21 = 0x24,
	OTP_PROG_GP22 = 0x28,
	OTP_GEN_RAND = 0x34
};

enum otp_state {
	OTP_SUCCESS = 0x00,
	OTP_PROGRAM_ERROR = 0x20,
	OTP_COMPARE_ERROR = 0x21
};

enum otp_api_error {
	OTP_WR_ENABLE_INVALID = 0x70001,
	OTP_SOME_BITS_ALREADY_PROGRAMMED = 0x70002,
	OTP_ALL_DATAORMASK_ZERO = 0x70003,
	OTP_WRITE_ACCESS_LOCKED = 0x70004,
	OTP_USB_ID_ENABLED = 0x70006,
};

extern struct boot_handler otp_handler;
#endif

