#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"

gap_buffer *gap_buffer_new(size_t initial_size)
{
    gap_buffer *gb = malloc(sizeof(gap_buffer));

    if(!gb) {
        fprintf(stderr, "Failed to allocate memory for gap_buffer.\n");
        return NULL;
    }
    gb->buffer = malloc(initial_size);
    if(!gb->buffer) {
        fprintf(stderr, "Failed to allocate memory for buffer.\n");
        free(gb);
        return NULL;
    }

    gb->buffer_size = initial_size;
    gb->gap_left = gb->buffer;
    gb->gap_right = gb->buffer + gb->buffer_size - 1;

    return gb;
}

void gap_buffer_destroy(gap_buffer *gb)
{
    free(gb->buffer);
    free(gb);
}

static int gap_buffer_grow(gap_buffer *gb)
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
    while(len >= gap_size) {
        int ret_code = gap_buffer_grow(gb);
        if(ret_code == -1) {
            fprintf(stderr, "Failed to grow gap size.");
            return -1;
        }
        gap_size = gb->gap_right - gb->gap_left + 1;
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

char *gap_buffer_get_text(gap_buffer *gb)
{
    size_t left_length = (gb->gap_left - gb->buffer);
    size_t right_length = (gb->buffer + gb->buffer_size - 1 - gb->gap_right);
    size_t text_length = left_length + right_length;
    
    char *clear_text = malloc(text_length);
    if(!clear_text) {
        fprintf(stderr, "Failed to allocate memory for clear text.\n");
        return NULL;
    }
    memcpy(clear_text, gb->buffer, left_length);
    memcpy(clear_text + left_length, gb->gap_right + 1, right_length);

    return clear_text;
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
