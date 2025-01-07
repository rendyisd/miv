#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#define ONE_KB 1024

typedef struct gap_buffer gap_buffer;

gap_buffer* gap_buffer_new();
void gap_buffer_grow(gap_buffer *);
void gap_buffer_destroy(gap_buffer *);

#endif
