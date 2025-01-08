#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gap_buffer.h"

/*
      The entirety of the gap_buffer
      _______________________________
     |                               |
     Hello, ____________________world!
            ^                  ^
            |                  |
         gap_left          gap_right
*/

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
    gb->gap_right = gb->buffer + gb->buffer_size - 1;
    

    return gb;
}

void gap_buffer_destroy(gap_buffer *gb)
{
    free(gb->buffer);
    free(gb);
}

/* Grow the gap_buffer by 1 KB */
void gap_buffer_grow(gap_buffer *gb)
{ 
    size_t gap_left_offset = gb->gap_left - gb->buffer;
    size_t gap_right_offset = gb->gap_right - gb->buffer;
    size_t right_side_size = gb->buffer_size - gap_right_offset - 1;

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
    memmove(gb->gap_right + 1, gb->buffer + gap_right_offset, right_side_size);
}

void gap_buffer_insert(gap_buffer *gb, char *text, size_t len)
{
    size_t gap_size = gb->gap_right - gb->gap_left + 1; 
    if(len >= gap_size) {
        gap_buffer_grow(gb);
        // TODO: Make a better way to handle error. Error not yet handled if the function above failed.
    }
    memcpy(gb->gap_left, text, len);
    gb->gap_left += len;
}

void gap_buffer_delete(gap_buffer *gb, size_t len, int direction)
{
    if(direction == D_LEFT) {
        // Prevents gap_left from going beyond buffer
        if(gb->gap_left - len >= gb->buffer) 
            gb->gap_left -= len;
    }
    else if(direction == D_RIGHT) {
        // Prevents gap_right from going beyond the tail of buffer
        if(gb->gap_right + len <= gb->buffer + gb->buffer_size - 1) 
            gb->gap_right += len;
    }
} 

void gap_buffer_debug(gap_buffer *gb)
{
    printf("Buffer address: %p\n", gb->buffer);
    printf("Gap left address: %p\n", gb->gap_left);
    printf("Gap right address: %p\n", gb->gap_right);
}
