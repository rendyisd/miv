#ifndef MIV_H
#define MIV_H

#include "buffer.h"

void enable_raw_mode();
void prepare_screen();
int redraw_screen(struct miv_row *);

#endif
