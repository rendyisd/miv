#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <termios.h>

#include "miv.h"
#include "buffer.h"

/*
 * miv.c
 * This file handles all edtior screen (terminal) behavior
 */

static struct termios orig_termios;

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

    //raw.c_lflag &= ~(ECHO | ICANON | ISIG); 
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

static int get_cursor_position(unsigned int *rows, unsigned int *cols)
{
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) /* ANSI escape sequences: request cursor postion (reports as /x1b[#;#R ) */
        return -1;

    while (i < sizeof(buf) - 1) {
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
    gap_buffer_move_gap_to_start(vp->on_cursor->gb);
    get_screen_size(&vp->nrows, &vp->ncols);
    vp->xoffset = 0;
    vp->yoffset = 0;
    vp->cursorx = 1;
    vp->cursory = 1;

    write(STDOUT_FILENO, "\x1b[H", 3); /* ANSI escape sequences: Move cursor to the top left */

    return vp;
}

static void screen_buffer_lazy_render(struct miv_viewport *vp, struct screen_buffer *sb)
{
    struct miv_row *curr_mr = vp->top_on_screen;
    for (unsigned int i = 0; i < vp->nrows; ++i) {
        screen_buffer_append(sb, "\x1b[K", 3);
        if (!curr_mr) { 
            screen_buffer_append(sb, "\r\n", 2);
            continue;
        }

        char *text = gap_buffer_get_text(curr_mr->gb);
        if (!text)
            return;
        size_t text_len = curr_mr->text_len;
        size_t remaining_text = (text_len < vp->xoffset) ? 0 : text_len - vp->xoffset;
        size_t size_to_append = (text_len > vp->ncols) ? vp->ncols : remaining_text;

        screen_buffer_append(sb, &text[vp->xoffset], size_to_append);

        free(text);

        curr_mr = curr_mr->next;

        screen_buffer_append(sb, "\r\n", 2);
    }
}

int cursor_debug(struct miv_viewport *vp, struct screen_buffer *sb)
{
    char tmp_buf[64];
    snprintf(tmp_buf, sizeof(tmp_buf), "X: %d | Y: %d | X offset: %d | Y offset: %d\r\nstrlen: %zu", vp->cursorx, vp->cursory, vp->xoffset, vp->yoffset, vp->on_cursor->text_len);
    screen_buffer_append(sb, tmp_buf, strlen(tmp_buf));

    return 0;
}

int render_screen(struct miv_viewport *vp)
{
    if (!vp)
        return -1;
 
    struct screen_buffer sb = SCREEN_BUFFER_INIT;

    screen_buffer_append(&sb, "\x1b[?25l\x1b[H", 9);
    screen_buffer_lazy_render(vp, &sb);
    cursor_debug(vp, &sb);
    
    char tmp_buf[32];
    snprintf(tmp_buf, sizeof(tmp_buf), "\x1b[%d;%dH\x1b[?25h", vp->cursory, vp->cursorx);
    screen_buffer_append(&sb, tmp_buf, strlen(tmp_buf));

    write(STDOUT_FILENO, sb.buffer, sb.len);
    
    screen_buffer_destroy(&sb);

    return 0;
}

