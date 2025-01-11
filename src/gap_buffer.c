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

gap_buffer *gap_buffer_new()
{
    gap_buffer *gb = malloc(sizeof(gap_buffer));

    if(!gb) {
        fprintf(stderr, "Failed to allocate memory for gap_buffer.\n");
        return NULL;
    }
    /* Init the buffer with the size of 1 KB */
    gb->buffer = malloc(GAP_SIZE);
    if(!gb->buffer) {
        fprintf(stderr, "Failed to allocate memory for buffer.\n");
        free(gb);
        return NULL;
    }

    gb->buffer_size = GAP_SIZE;
    gb->gap_left = gb->buffer;
    gb->gap_right = gb->buffer + gb->buffer_size - 1;

    return gb;
}

void gap_buffer_destroy(gap_buffer *gb)
{
    free(gb->buffer);
    free(gb);
}

int gap_buffer_grow(gap_buffer *gb)
{ 
    size_t gap_left_offset = gb->gap_left - gb->buffer;
    size_t gap_right_offset = gb->gap_right - gb->buffer;
    size_t right_side_size = gb->buffer_size - gap_right_offset - 1;

    void *temp = realloc(gb->buffer, gb->buffer_size + GAP_SIZE);
    if(!temp) {
        fprintf(stderr, "Failed to realloc memory.");
        return -1;
    }    

    gb->buffer = temp;
    gb->buffer_size += GAP_SIZE;
    gb->gap_left = gb->buffer + gap_left_offset;
    gb->gap_right = gb->buffer + gap_right_offset + GAP_SIZE;

    //Move the right-side content to its new place
    memmove(gb->gap_right + 1, gb->buffer + gap_right_offset + 1, right_side_size);
    return 0;
}

int gap_buffer_insert(gap_buffer *gb, char *text, size_t len)
{
    size_t gap_size = gb->gap_right - gb->gap_left + 1; 
    if(len >= gap_size) {
        int ret_code = gap_buffer_grow(gb);
        if(ret_code == -1) {
            fprintf(stderr, "Failed to grow gap size.");
            return -1;
        }
    }
    memcpy(gb->gap_left, text, len);
    gb->gap_left += len;
    return 0;
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

void gap_buffer_move_gap(gap_buffer *gb, size_t len, int direction)
{
    if(direction == D_LEFT) {
        if(gb->gap_left - len >= gb->buffer) {
            gb->gap_left -= len;
            gb->gap_right -= len;

            memcpy(gb->gap_right + 1, gb->gap_left, len);
        }
    }
    else if(direction == D_RIGHT) {
        if(gb->gap_right + len <= gb->buffer + gb->buffer_size - 1) {
            char *right_side_start = gb->gap_right + 1;

            gb->gap_left += len;
            gb->gap_right += len;

            memcpy(gb->gap_left - len, right_side_start, len);
        }
    }
}

void gap_buffer_print(gap_buffer *gb)
{
    char *c_ptr = gb->buffer;

    while(c_ptr != gb->buffer + gb->buffer_size) {
        if((c_ptr >= gb->gap_left) && (c_ptr <= gb->gap_right)) {
            printf("_ ");
        }
        else {
            printf("%c ", *c_ptr);
        }
        c_ptr += 1;
    }
    printf("\n");
}
    
void gap_buffer_debug(gap_buffer *gb)
{
    printf("Buffer address: %p\n", gb->buffer);
    printf("Gap left address: %p\n", gb->gap_left);
    printf("Gap right address: %p\n", gb->gap_right);
    printf("Gap buffer size: %zu\n", gb->buffer_size);
}
