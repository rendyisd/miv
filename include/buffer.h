#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>

#define GAP_SIZE 16

#define D_LEFT 1
#define D_RIGHT 2

/*
   The entirety of the struct gap_buffer
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

struct miv_row {
        struct gap_buffer *gb;
        size_t text_len;
        struct miv_row *prev;
        struct miv_row *next;
};

struct screen_buffer {
        char *buffer;
        size_t len;
};

#define SCREEN_BUFFER_INIT {NULL, 0}

/* Doubly linked list that represents each row. Each row is separated by a newline */

/* Create a new struct gap_buffer and return its pointer */
struct gap_buffer *gap_buffer_new(size_t);
/* Free allocated memory of struct gap_buffer */
void gap_buffer_destroy(struct gap_buffer *);
/* Insert text of size_t, text is always inserted to the left side of gap */
int gap_buffer_insert(struct gap_buffer *, char *, size_t);
/* Delete text of size_t, with D_LEFT and D_RIGHT to delete text to the left/right side of gap */
void gap_buffer_delete(struct gap_buffer *, size_t, int);
/* Move gap, or cursor(?) in text editor context */
void gap_buffer_move_gap(struct gap_buffer *, size_t, int);
void gap_buffer_move_gap_to_start(struct gap_buffer *);
void gap_buffer_move_gap_to_end(struct gap_buffer *);
/* Get clear text from struct gap_buffer */
char *gap_buffer_get_text(struct gap_buffer *);
/* Print buffer content for debugging purpose */
void gap_buffer_print(struct gap_buffer *);
/* Just a debug function */
void gap_buffer_debug(struct gap_buffer *);


struct miv_row *miv_row_new();
void miv_row_destroy(struct miv_row *);


int screen_buffer_append(struct screen_buffer *, const char *, size_t);
void screen_buffer_destroy(struct screen_buffer *);

#endif
