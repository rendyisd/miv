#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>

#define ONE_KB 1024

#define D_LEFT 1
#define D_RIGHT 2

typedef struct gap_buffer gap_buffer;

gap_buffer* gap_buffer_new();
void gap_buffer_grow(gap_buffer *);
void gap_buffer_destroy(gap_buffer *);
void gap_buffer_debug(gap_buffer *);
void gap_buffer_insert(gap_buffer *, char *, size_t);
void gap_buffer_delete(gap_buffer *, size_t, int);

#endif
