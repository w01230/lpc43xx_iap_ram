/* *
 * crc32 caculator
 *
 *
 * used for data check
 */
 
 #include "crc32.h"
 
 
unsigned long cacuate_crc32(unsigned char *data, unsigned long size, unsigned long crc)
{
	int i = 0; 
	int j = 0;
	const unsigned long polynomial = 0xEDB88320;
	unsigned int table[256] = {0x00};
	
	/* make table */
	for (i = 0 ; i < 256 ; i++) {
		for (j = 0, table[i] = i ; j < 8 ; j++)
			table[i] = (table[i]>>1)^((table[i]&1) ? polynomial : 0);
	}
	
	crc = ~crc;
	for (i = 0; i < size; i++)
		crc = (crc >> 8) ^ table[(crc ^ data[i]) & 0xff];
	
	return ~crc;
}
