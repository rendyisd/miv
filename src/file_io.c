#include <stdio.h>

#include "file_io.h"

//void temporary()
//{
//    FILE *file_ptr;
//    file_ptr = fopen("src/test_file.txt", "r");
//
//    if(!file_ptr) {
//        fprintf(stderr, "Failed to open file.\n");
//        exit(EXIT_FAILURE);
//    }
//
//    if(fseek(file_ptr, 0L, SEEK_END) == -1) {
//        fprintf(stderr, "Failed to seek to the EOF.\n");
//        exit(EXIT_FAILURE);
//    }
//
//    long bufsize = ftell(file_ptr);
//    gap_buffer *file_gb = gap_buffer_new((size_t) bufsize);
//    char *temp_buffer = malloc((size_t)bufsize);
// 
//    if(fseek(file_ptr, 0L, SEEK_SET) == -1) {
//        fprintf(stderr, "Failed to seek to the starting of file.\n");
//        exit(EXIT_FAILURE);
//    }
//    
//    size_t size_written = fread(temp_buffer, sizeof(char), bufsize, file_ptr);
//    if(ferror(file_ptr) != 0) {
//        fprintf(stderr, "Failed reading to buffer");
//        exit(EXIT_FAILURE);
//    }
//    // Has to be this way because gap_buffer is not defined in .h for this file to access directly into its buffer.
//    // Possible solutions are: 1) define gap_buffer in its header. 2) make a function that returns pointer to buffer
//    // Or just let it be like this, but it essentially reading the file twice
//    gap_buffer_insert(file_gb, temp_buffer, bufsize); 
//    gap_buffer_print(file_gb);
//    gap_buffer_debug(file_gb);
//
//    free(temp_buffer);
//    fclose(file_ptr);
//    gap_buffer_destroy(file_gb);
//}

int open_file_to_buffer(char *filename)
{
    FILE *fhandle = fopen(filename, "r");

    if (!fhandle)
        return -1;
    
     

    return 0;
}
