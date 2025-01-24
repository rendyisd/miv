#include <stdio.h>

/*
 * input.c
 * Handles keyboard and mouse(? probably not anytime soon) input
 */

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
        seq[2] = getchar();

        if (seq[0] == '[') {
            if ((seq[1] >= '1' && seq[1] <= '8') && seq[2] == '~') {
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

int handle_keypress(struct miv_viewport *vp)
{
    int c = read_keypress();
    
    /* TODO: Newline, also if done in the middle of a line, move right content to newline */
    switch (c) {
    case KEY_BACKSPACE:
        break;
    case KEY_TAB:
        break;
    case KEY_ENTER:
        break;
    case KEY_DELETE:
        break;
    case KEY_HOME:
        /* Move cursor to start of line */
        break;
    case KEY_END:
        /* Move  cursor to end of line */
        break;
    case KEY_PAGEUP:
        /* */
        break;
    case KEY_PAGEDOWN:
        /* */
        break;
    case KEY_ARROW_UP:
        /* */
        break;
    case KEY_ARROW_DOWN:
        /* */
        break;
    case KEY_ARROW_LEFT:
        /* */
        break;
    case KEY_ARROW_RIGHT:
        /* */
        break;
    default:
        /* Something-something insert to miv_row gap_buffer */
        break;
    }
}
