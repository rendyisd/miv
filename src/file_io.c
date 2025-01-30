#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_io.h"
#include "buffer.h"

static char *read_line(FILE *fp)
{
    char *buf = NULL;
    char tmpbuf[32];
    
    size_t total_buf_len = 0;
    unsigned short int is_newline = 0;
    while (is_newline == 0) {
        fgets(tmpbuf, 32, fp);
        size_t tmpbuf_len = strlen(tmpbuf);

        if (tmpbuf[tmpbuf_len - 1] == '\n' || feof(fp))
            is_newline = 1;

        char *new_buf = realloc(buf, total_buf_len + tmpbuf_len + is_newline); // is_newline is used to reserve space for null terminator
        if (!new_buf) {
            free(buf);
            fprintf(stderr, "Failed to reallocate data for read_line.");
            return NULL;
        }
        buf = new_buf;
        memcpy(buf + total_buf_len, tmpbuf, tmpbuf_len);
        total_buf_len += tmpbuf_len;
    }

    if (buf)
        buf[total_buf_len] = '\0';

    return buf;
}

struct miv_row *open_file_to_buffer(char *filename)
{
    FILE *fhandle = fopen(filename, "r");

    if (!fhandle)
        return NULL;
    
    struct miv_row *mr_head = miv_row_new();
    struct miv_row *mr_prev = NULL;
    struct miv_row *mr = NULL;
    while (!feof(fhandle)) {
        if (!mr) 
            mr = mr_head;
        else
            mr = miv_row_new();
        
        char *line = read_line(fhandle);
        size_t line_len = strlen(line);
        gap_buffer_insert(mr->gb, line, line_len);
        mr->text_len = line_len;
        mr->prev = mr_prev;
        if(mr_prev)
            mr_prev->next = mr;
        
        mr_prev = mr;

        free(line);
    }
    fclose(fhandle);

    return mr_head;
}
