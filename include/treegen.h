#ifndef _TREEGEN_H
#define _TREEGEN_H
#include <stdint.h>
#include <stdlib.h>
typedef struct {
    void *buffer;
    size_t pos;
    size_t cap;
} Arena;

void make_tree(Arena *a, uint32_t *out_buffer);
Arena arena_new(size_t max_bytes);

#endif
