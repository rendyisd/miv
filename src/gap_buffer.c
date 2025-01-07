#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gap_buffer.h"

struct gap_buffer {
    char *buffer;
    size_t buffer_size;
    char *gap_left;
    char *gap_right;
};

gap_buffer* gap_buffer_new()
{
    gap_buffer *gb = malloc(sizeof(gap_buffer));

    if(!gb) {
        fprintf(stderr, "Failed to allocate memory for gap_buffer.\n");
        return NULL;
    }
    /* Init the buffer with the size of 1 KB */
    gb->buffer = malloc(ONE_KB);
    if(!gb->buffer) {
        fprintf(stderr, "Failed to allocate memory for buffer.\n");
        free(gb);
        return NULL;
    }

    gb->buffer_size = ONE_KB;
    gb->gap_left = gb->buffer;
    gb->gap_right = gb->buffer + ONE_KB;

    return gb;
}

/* Grow the gap_buffer by 1 KB */
void gap_buffer_grow(gap_buffer *gb)
{ 
    size_t gap_left_offset = gb->gap_left - gb->buffer;
    size_t gap_right_offset = gb->gap_right - gb->buffer;
    size_t right_side_size = gb->buffer_size - gap_right_offset;

    void *temp = realloc(gb->buffer, gb->buffer_size + ONE_KB);
    if(!temp) {
        fprintf(stderr, "Failed to realloc memory.");
        return;
    }    

    gb->buffer = temp;
    gb->buffer_size += ONE_KB;
    gb->gap_left = gb->buffer + gap_left_offset;
    gb->gap_right = gb->buffer + gap_right_offset + ONE_KB;

    //Move the right-side content to its new place
    memmove(gb->gap_right, gb->buffer + gap_right_offset, right_side_size);
}

void gap_buffer_destroy(gap_buffer *gb)
{
    free(gb->buffer);
    free(gb);
}
