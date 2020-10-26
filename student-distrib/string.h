/* String utils */
#include "types.h"

int32_t string_length(const uint8_t* s);
/* Calculate whether two strings are equal */
int32_t string_equal(const uint8_t* s1, const uint8_t* s2);
/* Return a substring of the input string */
int32_t substring(const uint8_t* s, uint8_t* buf, uint32_t start, uint32_t end);
/* prints buffer data char by char */
void print_buf(uint8_t* buf, uint32_t bytes);
