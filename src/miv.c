#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>

#include "buffer.h"

/*
 * miv.c
 * This file handles all edtior screen (terminal) behavior
 */

static struct termios orig_termios;

struct miv_viewport {
    struct miv_row *top_on_screen;
    struct miv_row *on_cursor;
    unsigned int nrows;
    unsigned int ncols;
    unsigned int xoffset;
    unsigned int yoffset;
    unsigned int cursorx;
    unsigned int cursory;
};

static void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

static void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON | ISIG); 
    //raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    //raw.c_oflag &= ~(OPOST);
    //raw.c_cflag |= (CS8);
    //raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    //raw.c_cc[VMIN] = 1;
    //raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static int get_cursor_position(unsigned int *rows, unsigned int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) /* ANSI escape sequences: request cursor postion (reports as /x1b[#;#R ) */
        return -1;
    while (i < sizeof(buf) - 1)
    {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if (buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

static int get_screen_size(unsigned int *rows, unsigned int *cols)
{
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) /* ANSI escape sequences: moves cursor 999 right and 999 down */
        return -1;
    return get_cursor_position(rows, cols);
}

static void destroy_screen()
{
    disable_raw_mode();
    write(STDOUT_FILENO, "\x1b[J", 3); /* ANSI escape sequences: Erase display */
    write(STDOUT_FILENO, "\x1b[?1049l", 8); /* ANSI escape sequences: Disables alternative screen buffer */
}

struct miv_viewport *prepare_screen(struct miv_row *mr)
{
    atexit(destroy_screen);
    write(STDOUT_FILENO, "\x1b[?1049h", 8); /* ANSI escape sequences: Enables alternative screen buffer */ 
    enable_raw_mode();

    struct miv_viewport *vp = malloc(sizeof(struct miv_viewport));
    vp->top_on_screen = mr;
    vp->on_cursor = mr;
    get_screen_size(&vp->nrows, &vp->ncols);
    vp->xoffset = 0;
    vp->yoffset = 0;
    vp->cursorx = 1;
    vp->cursory = 1;
    write(STDOUT_FILENO, "\x1b[H", 3); /* ANSI escape sequences: Move cursor to the top left */

    return vp;
}


/* TODO: WIP, not final functionality */
int render_screen(struct miv_row *mr_head)
{
    if (!mr_head)
        return -1;

    struct miv_row *mr = mr_head;
    do {
        printf("%s", gap_buffer_get_text(mr->gb)); 
        mr = mr->next;
    } while (mr);
    return 0;
}
