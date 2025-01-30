#ifndef MIV_H
#define MIV_H

#include "buffer.h"

#define MIV_TAB_STOP 4

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

void enable_raw_mode();
struct miv_viewport *prepare_screen(struct miv_row *);
int render_screen(struct miv_viewport *);

#endif
