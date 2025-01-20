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

void die(const char *s)
{
    perror(s);
    exit(EXIT_FAILURE);
}

static void disable_raw_mode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); 
    write(STDOUT_FILENO, "\x1b[?1049l", 8); // ANSI escape sequences: Disables alternative screen buffer
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
    //raw.c_cc[VMIN] = 0;
    //raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    write(STDOUT_FILENO, "\x1b[?1049h", 8); // ANSI escape sequences: Enables alternative screen buffer
}

int get_cursor_position(int *rows, int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) // ANSI escape sequences: request cursor postion (reports as /x1b[#;#R )
        return -1;

    while(i < sizeof(buf) - 1)
    {
        if(read(STDIN_FILENO, &buf[i], 1) != 1)
            break;
        if(buf[i] == 'R')
            break;
        i++;
    }
    buf[i] = '\0';

    if(buf[0] != '\x1b' || buf[1] != '[')
        return -1;
    if(sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return -1;

    return 0;
}

int get_screen_size(int *rows, int *cols)
{
    if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) // ANSI escape sequences: moves cursor 999 right and 999 down
        return -1;
    return get_cursor_position(rows, cols);
}
