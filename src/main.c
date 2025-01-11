#include <unistd.h>
#include <stdio.h>

#include "gap_buffer.h"

int main()
{
    gap_buffer *gb = gap_buffer_new();

    gap_buffer_print(gb);

    char *text_1 = "Hello";
    gap_buffer_insert(gb, text_1, 5);
    gap_buffer_print(gb);

    gap_buffer_move_gap(gb, 3, D_LEFT);
    gap_buffer_print(gb);
    
    char *text_2 = "y, bro";
    gap_buffer_insert(gb, text_2, 6);
    gap_buffer_print(gb);

    gap_buffer_delete(gb, 3, D_RIGHT);
    gap_buffer_print(gb);

    gap_buffer_move_gap(gb, 5, D_LEFT);
    gap_buffer_print(gb);

    char *text_3 = "12345678";
    gap_buffer_insert(gb, text_3, 8);
    gap_buffer_print(gb);

    gap_buffer_move_gap(gb, 5, D_RIGHT);
    gap_buffer_print(gb);

    printf("Clear text: %s\n", gap_buffer_get_text(gb));

    gap_buffer_debug(gb);

    gap_buffer_destroy(gb);

    return 0;
}
