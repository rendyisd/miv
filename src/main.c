#include <stdio.h>
#include <unistd.h>

#include "miv.h"
#include "buffer.h"
#include "file_io.h"

int main(int argc, char *argv[])
{
    prepare_screen();

    //if(argc >= 2) {
    //    miv_open_file(argv[1]);
    //}
    
    struct miv_row *mr_head = open_file_to_buffer("src/test_file.txt"); 
    redraw_screen(mr_head);

    char c;

    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        printf("Pressed: %c\n", c);
    }

    return 0;
}
