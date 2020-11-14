/* File util file, applying filesys functions */
#ifndef _FILE_UTIL_H
#define _FILE_UTIL_H
#include "../types.h"

/* Nicely print information about this file */
void print_file_info(uint8_t* fname);
/* Check if a file is executable, using its file name */
uint32_t is_executable(uint8_t* exe);

#endif /* _FILE_UTIL_H */
