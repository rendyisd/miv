#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"

struct gap_buffer *gap_buffer_new(size_t initial_size)
{
    struct gap_buffer *gb = malloc(sizeof(struct gap_buffer));

    if (!gb) {
        fprintf(stderr, "Failed to allocate memory for struct gap_buffer.\n");
        return NULL;
    }

    gb->buffer = malloc(initial_size);
    if (!gb->buffer) {
        fprintf(stderr, "Failed to allocate memory for buffer.\n");
        free(gb);
        return NULL;
    }

    gb->buffer_size = initial_size;
    gb->gap_left = gb->buffer;
    gb->gap_right = gb->buffer + gb->buffer_size - 1;

    return gb;
}

void gap_buffer_destroy(struct gap_buffer *gb)
{
    if (!gb)
        return;

    free(gb->buffer);
    free(gb);
}

static int gap_buffer_grow(struct gap_buffer *gb)
{ 
    size_t gap_left_offset = gb->gap_left - gb->buffer;
    size_t gap_right_offset = gb->gap_right - gb->buffer;
    size_t right_side_size = gb->buffer_size - gap_right_offset - 1;

    void *temp = realloc(gb->buffer, gb->buffer_size + GAP_SIZE);
    if (!temp) {
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

int gap_buffer_insert(struct gap_buffer *gb, char *text, size_t len)
{
    size_t gap_size = gb->gap_right - gb->gap_left + 1; 
    while (len >= gap_size) {
        int ret_code = gap_buffer_grow(gb);
        if (ret_code == -1) {
            fprintf(stderr, "Failed to grow gap size.");
            return -1;
        }
        gap_size = gb->gap_right - gb->gap_left + 1;
    }
    memcpy(gb->gap_left, text, len);
    gb->gap_left += len;
    return 0;
}

void gap_buffer_delete(struct gap_buffer *gb, size_t len, int direction)
{
    if (direction == D_LEFT) {
        // Prevents gap_left from going beyond buffer
        if (gb->gap_left - len >= gb->buffer) 
            gb->gap_left -= len;
    } else if (direction == D_RIGHT) {
        // Prevents gap_right from going beyond the tail of buffer
        if (gb->gap_right + len <= gb->buffer + gb->buffer_size - 1) 
            gb->gap_right += len;
    }
} 

void gap_buffer_move_gap(struct gap_buffer *gb, size_t len, int direction)
{
    if (direction == D_LEFT) {
        if (gb->gap_left - len >= gb->buffer) {
            gb->gap_left -= len;
            gb->gap_right -= len;

            memcpy(gb->gap_right + 1, gb->gap_left, len);
        }
    } else if (direction == D_RIGHT) {
        if (gb->gap_right + len <= gb->buffer + gb->buffer_size - 1) {
            char *right_side_start = gb->gap_right + 1;

            gb->gap_left += len;
            gb->gap_right += len;

            memcpy(gb->gap_left - len, right_side_start, len);
        }
    }
}
void gap_buffer_move_gap_to_start(struct gap_buffer *gb)
{
    size_t left_length = gb->gap_left - gb->buffer;
    gap_buffer_move_gap(gb, left_length, D_LEFT);
}
void gap_buffer_move_gap_to_end(struct gap_buffer *gb)
{
    size_t right_length = (gb->buffer + gb->buffer_size - 1 - gb->gap_right);
    //if (gb->buffer[gb->buffer_size - 1] == '\n' && right_length > 0)
    //    right_length--;
    gap_buffer_move_gap(gb, right_length, D_RIGHT);
}

/* 
 * WARNING: free char * when done 
 * Dispose of \n
 * */
char *gap_buffer_get_text(struct gap_buffer *gb)
{
    size_t left_length = (gb->gap_left - gb->buffer);
    size_t right_length = (gb->buffer + gb->buffer_size - 1 - gb->gap_right);
    size_t text_length = left_length + right_length;
    if (text_length == 0)
        return NULL;
    
    char *clear_text = malloc(text_length + 1); // Include null terminator
    if (!clear_text) {
        fprintf(stderr, "Failed to allocate memory for clear text.\n");
        return NULL;
    }
    memcpy(clear_text, gb->buffer, left_length);
    memcpy(clear_text + left_length, gb->gap_right + 1, right_length);
    clear_text[text_length] = '\0';

    return clear_text;
}
 
void gap_buffer_print(struct gap_buffer *gb)
{
    char *c_ptr = gb->buffer;

    while (c_ptr != gb->buffer + gb->buffer_size) {
        if ((c_ptr >= gb->gap_left) && (c_ptr <= gb->gap_right))
            printf("_ ");
        else
            printf("%c ", *c_ptr);

        c_ptr += 1;
    }
    printf("\n");
}
    
void gap_buffer_debug(struct gap_buffer *gb)
{
    printf("Buffer address: %p\n", gb->buffer);
    printf("Gap left address: %p\n", gb->gap_left);
    printf("Gap right address: %p\n", gb->gap_right);
    printf("Gap buffer size: %zu\n", gb->buffer_size);
}

struct miv_row *miv_row_new()
{
    struct miv_row *mr = malloc(sizeof(struct miv_row));
    if (!mr) {
        fprintf(stderr, "Failed to allocate memory for miv_row.\n");
        return NULL;
    }

    mr->gb = gap_buffer_new(GAP_SIZE);
    if (!mr->gb) {
        free(mr);
        return NULL;
    }
    mr->text_len = 0;
    mr->next = NULL;
    mr->prev = NULL;
    return mr;
}

void miv_row_destroy(struct miv_row *mr)
{
    if (!mr)
        return;
    
    if (mr->prev)
        mr->prev->next = mr->next;

    if (mr->next)
        mr->next->prev = mr->prev;

    gap_buffer_destroy(mr->gb);
    free(mr);    
}

int screen_buffer_append(struct screen_buffer *sb, const char *text, size_t len)
{
    char *tmp_buf = realloc(sb->buffer, sb->len + len); 
    if (!tmp_buf)
        return -1;
    memcpy(&tmp_buf[sb->len], text, len);
    sb->buffer = tmp_buf;
    sb->len += len;
    return 0;
}

void screen_buffer_destroy(struct screen_buffer *sb)
{
    free(sb->buffer);
}
