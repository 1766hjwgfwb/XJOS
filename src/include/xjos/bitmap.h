#ifndef XJOS_BITMAP_H
#define XJOS_BITMAP_H


#include <xjos/types.h>


typedef struct {
    u8 *bits;
    u32 length;
    u32 offset;
}bitmap_t;


void bitmap_init(bitmap_t *map, char *bits, u32 length, u32 start);

void bitmap_make(bitmap_t *map, char *bits, u32 length, u32 offset);

bool bitmap_test(bitmap_t *map, idx_t index);

void bitmap_set(bitmap_t *map, idx_t index, bool value);

int bitmap_scan(bitmap_t *map, u32 count);


#endif /* XJOS_BITMAP_H */