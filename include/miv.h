#ifndef MIV_H
#define MIV_H

#include "buffer.h"

#define MIV_TAB_STOP 4

struct miv_viewport;

void enable_raw_mode();
struct miv_viewport *prepare_screen(struct miv_row *);
int render_screen(struct miv_row *);

#endif
