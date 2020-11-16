/* String utils */
#ifndef _CHAR_UTIL_H
#define _CHAR_UTIL_H

#include "../types.h"

int32_t string_length(const uint8_t* s);
/* Calculate whether two strings are equal */
int32_t string_equal(const uint8_t* s1, const uint8_t* s2);
/* Return a substring of the input string */
int32_t substring(const uint8_t* s, uint8_t* buf, uint32_t start, uint32_t end);
/* Copies all contents of a string into another */
int32_t copy_string(const uint8_t* source, uint8_t* dest);
/* Copies a specific number of bytes from one buffer to another */
int32_t copy_buf(const uint8_t* source, uint8_t* dest, uint32_t nbytes);
/* Casts a string of chars into type uint8_t for most of our functions */
uint8_t* dechar(const char* s);
/* prints buffer data char by char */
void print_buf(uint8_t* buf, uint32_t bytes);

int32_t copy_buf_gradually(const uint8_t* source, uint8_t* dest, uint32_t nbytes);

#endif /* _CHAR_UTIL_H */
