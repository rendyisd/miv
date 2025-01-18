#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>

#define GAP_SIZE 16

#define D_LEFT 1
#define D_RIGHT 2

/*
      The entirety of the gap_buffer
      _______________________________
     |                               |
     Hello, ____________________world!
            ^                  ^
            |                  |
         gap_left          gap_right
*/

typedef struct gap_buffer gap_buffer;
typedef struct miv_row miv_row;

struct gap_buffer {
    char *buffer;
    size_t buffer_size;
    char *gap_left;
    char *gap_right;
};

struct miv_row {
    gap_buffer *gb;
    miv_row *prev;
    miv_row *next;
};

typedef struct gap_buffer gap_buffer;
/* Doubly linked list that represents each row. Each row is separated by a newline */
typedef struct miv_row miv_row;

/* Create a new gap_buffer and return its pointer */
gap_buffer *gap_buffer_new(size_t);
/* Free allocated memory of gap_buffer */
void gap_buffer_destroy(gap_buffer *);
/* Insert text of size_t, text is always inserted to the left side of gap */
int gap_buffer_insert(gap_buffer *, char *, size_t);
/* Delete text of size_t, with D_LEFT and D_RIGHT to delete text to the left/right side of gap */
void gap_buffer_delete(gap_buffer *, size_t, int);
/* Move gap, or cursor(?) in text editor context */
void gap_buffer_move_gap(gap_buffer *, size_t, int);
/* Get clear text from gap_buffer */
char *gap_buffer_get_text(gap_buffer *);


/* Print buffer content for debugging purpose */
void gap_buffer_print(gap_buffer *);
/* Just a debug function */
void gap_buffer_debug(gap_buffer *);

#endif
