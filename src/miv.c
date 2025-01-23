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
    struct miv_row *top_on_display;
    int n_rows;
    int n_cols;
};

static void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

static void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
    write(STDOUT_FILENO, "\x1b[?1049l", 8); /* ANSI escape sequences: Disables alternative screen buffer */
}

void enable_raw_mode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

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

static int get_cursor_position(int *rows, int *cols)
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

static int get_screen_size(int *rows, int *cols)
{
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) /* ANSI escape sequences: moves cursor 999 right and 999 down */
        return -1;
    return get_cursor_position(rows, cols);
}

void prepare_screen()
{
    write(STDOUT_FILENO, "\x1b[?1049h", 8); /* ANSI escape sequences: Enables alternative screen buffer */ 
    enable_raw_mode();

    int *rows = malloc(sizeof(int));
    int *cols = malloc(sizeof(int));

    get_screen_size(rows, cols);
    write(STDOUT_FILENO, "\x1b[H", 3); /* ANSI escape sequences: Move cursor to the top left */
    //printf("Rows: %d | Cols: %d\n", *rows, *cols);

    free(rows);
    free(cols);
}

/* TODO: WIP, not final functionality */
int redraw_screen(struct miv_row *mr_head)
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
