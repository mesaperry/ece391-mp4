#include "file_util.h"
#include "../types.h"
#include "../filesys.h"
#include "../lib.h"
#include "char_util.h"

#define PRINT_FILE_SIZE_LEN 8

/* file name: 012345678901234567890123456789012 file_type: #, file_size: ########   */
/* ________________________________________________________________________________ */

void print_file_info(uint8_t* fname)
{
    int i;

    /* Write buffers */
    dentry_t dentry;
    read_dentry_by_name(fname, &dentry);

    printf("file name: ");

    uint32_t file_name_length = string_length(fname);
    for (i = 0; i < FNAME_MAX_LEN - file_name_length; i++) printf(" ");
    printf(fname);

    printf(" file type: ");
    printf("%d", dentry.file_type);

    printf(" file size: ");

    /* Try to align file size right but skip for now */
    // int j;
    // uint32_t size;
    // for (i = 0; i < 8; i++) {
    //     size = file_size(fname);
    //     for (j = 0; j < 8 - i; j++) size /= 10;
    //     size %= 10;
    //     if (size == 0) printf(" ");
    //     else printf("%d", size);
    // }
    printf("%d", file_size(fname));
    printf("\n");
}
