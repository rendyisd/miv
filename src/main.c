#include <unistd.h>
#include <stdio.h>

#include "gap_buffer.h"

int main()
{
    gap_buffer *gb = gap_buffer_new();
    gap_buffer_destroy(gb);
    printf("Gap buffer test.\n");

    return 0;
}
