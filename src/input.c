#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * input.c
 * Handles keyboard and mouse(? probably not anytime soon) input
 */
#include "miv.h"

enum Key {
        KEY_BACKSPACE = 8,
        KEY_TAB = 9,
        KEY_ENTER = 10, // Or 13 for carriage return
        KEY_DELETE = 127,
        KEY_HOME = 1000,
        KEY_END,
        KEY_PAGEUP,
        KEY_PAGEDOWN,
        KEY_ARROW_UP,
        KEY_ARROW_DOWN,
        KEY_ARROW_LEFT,
        KEY_ARROW_RIGHT
};

static int read_keypress()
{
        char first_c;
        if ((first_c = getchar()) == EOF)
                return -1;

        if (first_c == '\x1b') {
                char seq[3];
                seq[0] = getchar();
                seq[1] = getchar();

                if (seq[0] == '[') {
                        if (seq[1] >= '1' && seq[1] <= '8') {
                                seq[2] = getchar();
                                if (seq[2] == '~') {
                                        switch (seq[1]) {
                                                case '1':
                                                        return KEY_HOME;
                                                case '3':
                                                        return KEY_DELETE;
                                                case '4':
                                                        return KEY_END;
                                                case '5':
                                                        return KEY_PAGEUP;
                                                case '6':
                                                        return KEY_PAGEDOWN;
                                                case '7':
                                                        return KEY_HOME;
                                                case '8':
                                                        return KEY_END;
                                        }
                                }
                        } else if (seq[1] >= 'A' && seq[1] <= 'H') {
                                switch (seq[1]) {
                                        case 'A':
                                                return KEY_ARROW_UP;
                                        case 'B':
                                                return KEY_ARROW_DOWN;
                                        case 'C':
                                                return KEY_ARROW_RIGHT;
                                        case 'D':
                                                return KEY_ARROW_LEFT;
                                        case 'F':
                                                return KEY_END;
                                        case 'H':
                                                return KEY_HOME;
                                }
                        }
                }
        }

        return first_c;
}

void arrow_move_cursor(struct miv_viewport *vp, int key)
{
        switch (key) {
                case KEY_ARROW_UP:
                        if (!vp->on_cursor->prev)
                                return;

                        if (vp->cursory > 1) {
                                vp->cursory--;
                        } else {
                                vp->yoffset--;
                                vp->top_on_screen = vp->top_on_screen->prev;
                        }

                        vp->on_cursor = vp->on_cursor->prev;

                        if (vp->cursorx > vp->on_cursor->text_len + 1)
                                vp->cursorx = vp->on_cursor->text_len + 1;

                        gap_buffer_move_gap_absolute(vp->on_cursor->gb, vp->cursorx - 1);

                        break;

                case KEY_ARROW_DOWN:
                        if (!vp->on_cursor->next)
                                return;

                        if (vp->cursory < vp->nrows - N_RESERVED_ROW) {
                                vp->cursory++;
                        } else {
                                vp->yoffset++;
                                vp->top_on_screen = vp->top_on_screen->next;
                        }

                        vp->on_cursor = vp->on_cursor->next;

                        if (vp->cursorx > vp->on_cursor->text_len + 1)
                                vp->cursorx = vp->on_cursor->text_len + 1;

                        gap_buffer_move_gap_absolute(vp->on_cursor->gb, vp->cursorx - 1);
                        break;

                case KEY_ARROW_LEFT:
                        if (vp->xoffset == 0 && vp->cursorx == 1)
                                return;

                        if (vp->cursorx > 1)
                                vp->cursorx--;
                        else if (vp->xoffset > 0)
                                vp->xoffset--;

                        gap_buffer_move_gap_relative(vp->on_cursor->gb, 1, D_LEFT);
                        break;

                case KEY_ARROW_RIGHT:
                        if (vp->xoffset + vp->cursorx >= vp->on_cursor->text_len + 1)
                                return;

                        if (vp->cursorx < vp->ncols)
                                vp->cursorx++;
                        else
                                vp->xoffset++;

                        gap_buffer_move_gap_relative(vp->on_cursor->gb, 1, D_RIGHT);
                        break;
        }
}

int handle_keypress(struct miv_viewport *vp)
{
        int c = read_keypress();

        /* TODO: Newline, also if done in the middle of a line, move right content to newline */
        switch (c) {
                case KEY_BACKSPACE:
                        /* TODO: CHECK FOR NEWLINE CASE */
                        break;
                case KEY_TAB:
                        break;
                case KEY_ENTER:
                        break;
                case KEY_DELETE:
                        /* TODO: CHECK FOR NEWLINE CASE */
                        exit(EXIT_SUCCESS); // For debugging purpose
                        break;

                case KEY_HOME:
                        vp->cursorx = 1;
                        vp->xoffset = 0;
                        gap_buffer_move_gap_to_start(vp->on_cursor->gb);
                        break;

                case KEY_END:
                        vp->cursorx = (vp->on_cursor->text_len < vp->ncols) ? \
                                      vp->on_cursor->text_len + 1 : vp->ncols;
                        vp->xoffset = (vp->on_cursor->text_len < vp->ncols) ? \
                                      0 : vp->on_cursor->text_len - vp->ncols + 1;
                        gap_buffer_move_gap_to_end(vp->on_cursor->gb);
                        break;

                case KEY_PAGEUP:
                        /* */
                        break;

                case KEY_PAGEDOWN:
                        /* */
                        break;

                case KEY_ARROW_UP:
                case KEY_ARROW_DOWN:
                case KEY_ARROW_LEFT:
                case KEY_ARROW_RIGHT:
                        arrow_move_cursor(vp, c);
                        break;

                default: ;
                         char temp[2];
                         snprintf(temp, 2, "%c", c);
                         gap_buffer_insert(vp->on_cursor->gb, temp, 1);
                         vp->on_cursor->text_len++;

                         if (vp->cursorx < vp->ncols) {
                                 vp->cursorx++;
                         } else{
                                 vp->xoffset++;
                         }

                         break;
        }

        return 0;
}
