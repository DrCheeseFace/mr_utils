#ifndef MRM_MISC_H
#define MRM_MISC_H

#include <stdlib.h>
#define internal static
#define local_persist static
#define global_variable static

#define MRM_CAFE_BABE 0xCAFEBABE
#define MRM_CAFE_BABE_BYTE_COUNT 4

void MRM_set_cafebabe_bytes(void *ptr, size_t size);

#endif // !MRM_MISC_H
