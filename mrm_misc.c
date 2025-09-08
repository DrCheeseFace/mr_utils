#include "mrm_misc.h"

unsigned const char MRM_CAFE_BABE_BYTES[MRM_CAFE_BABE_BYTE_COUNT] = {
	0xCA, 0xFE, 0xBA, 0xBE
};

void MRM_set_cafebabe_bytes(void *ptr, size_t size)
{
	unsigned char *bytes = ptr;
	size_t cafe_babe_byte_idx = 0;
	for (size_t i = 0; i < size; i++) {
		bytes[i] = MRM_CAFE_BABE_BYTES[cafe_babe_byte_idx];
		cafe_babe_byte_idx++;
		if (cafe_babe_byte_idx == MRM_CAFE_BABE_BYTE_COUNT) {
			cafe_babe_byte_idx = 0;
		}
	}
}
