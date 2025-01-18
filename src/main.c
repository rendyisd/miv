#include <stdio.h>
#include <unistd.h>
#include "miv.h"

int main(int argc, char *argv[])
{
    enable_raw_mode();
    write(STDOUT_FILENO, "\x1b[2J", 4); // ANSI escape sequences: Clear terminal
    write(STDOUT_FILENO, "\x1b[H", 3); // ANSI escape sequences: Move cursor to the top left

    char c;

    while(read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        printf("Pressed: %c\n", c);
    }

    return 0;
}
