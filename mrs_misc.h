#ifndef MRS_MISC_H
#define MRS_MISC_H

#define internal static
#define local_persist static
#define global_variable static

#define CAFE_BABE 0xCAFEBABE
#define CAFE_BABE_BYTE_COUNT 4
unsigned const char CAFE_BABE_BYTES[CAFE_BABE_BYTE_COUNT] = { 0xCA, 0xFE, 0xBA,
							      0xBE };

#endif // !MRS_MISC_H
