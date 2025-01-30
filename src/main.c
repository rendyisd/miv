#include <stdio.h>

#include "miv.h"
#include "buffer.h"
#include "file_io.h"
#include "input.h"

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    struct miv_row *mr_head = open_file_to_buffer("src/test_file.txt"); 
    struct miv_viewport *vp = prepare_screen(mr_head);

    //if(argc >= 2) {
    //    miv_open_file(argv[1]);
    //}
    
    while (1) {
        render_screen(vp); 
        handle_keypress(vp);
    }
    
    return 0;
}
