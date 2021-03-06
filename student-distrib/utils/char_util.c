#include "char_util.h"
#include "../lib.h"

/* string_length
 * DESCRIPTION: Give the length of an input string, not including EOS
 * INPUT: s -- string input
 * OUTPUT: none
 * RETURNS: length of string, or -1 if fail
 * SIDE EFFECTS: none
 */
int32_t string_length(const uint8_t* s)
{
    if (s == NULL) return -1;

    int32_t count = 0;
    while (s[count] != '\0') {
        count++;
        if (count > 1000000) return -1;
    }
    return count;
}

/* string_equal
 * DESCRIPTION: Check if two strings are equal
 * INPUT: s1 -- first string input
 *        s2 -- second string input
 * OUTPUT: none
 * RETURNS: 1 if equal, 0 if not equal, -1 if fail
 * SIDE EFFECTS: none
 */
int32_t string_equal(const uint8_t* s1, const uint8_t* s2)
{
    if ((s1 == NULL) || (s2 == NULL)) return -1;

    int32_t i;
    for (i = 0; (s1[i] != '\0') && (s2[i] != '\0'); i++) {
        if (s1[i] != s2[i]) return 0;
    }
    if ((s1[i] == '\0') && (s2[i] == '\0')) {
        return 1;
    }
    return 0;
}

/* substring
 * DESCRIPTION: Copy in a portion of a string into a buffer
 * INPUT: s -- original string, which doesn't change
 *        buf   -- the buffer in which to store the substring
 *        start -- the start of the substring, inclusive
 *        end   -- the end of the substring, exclusive
 * OUTPUT: none
 * RETURNS: -1 if failure, 0 if success
 * SIDE EFFECTS: none
 */
int32_t substring(const uint8_t* s, uint8_t* buf, uint32_t start, uint32_t end)
{
    if ((s == NULL) || (buf == NULL)) return -1;
    int32_t len = string_length(s);
    if ((start < 0) || (end < start) || (end > len)) return -1;

    int32_t i;
    for (i = start; i < end; i++) {
        buf[i - start] = s[i];
    }
    buf[end - start] = '\0';
    return 0;
}

/* copy_string
 * DESCRIPTION: Copies a string into another
 * INPUT: source -- original string, which doesn't change
 *        dest   -- the destination string location
 * OUTPUT: none
 * RETURNS: -1 if failure, 0 if success
 * SIDE EFFECTS: none
 */
int32_t copy_string(const uint8_t* source, uint8_t* dest)
{
    if ((source == NULL) || (dest == NULL)) return -1;
    int32_t len = string_length(source);

    int32_t i;
    for (i = 0; i < len; i++) {
        dest[i] = source[i];
    }
    dest[len] = '\0';
    return 0;
}

/* Same as copy string, but with an explicit number of bytes instead */
int32_t copy_buf(const uint8_t* source, uint8_t* dest, uint32_t nbytes)
{
    if ((source == NULL) || (dest == NULL)) return -1;
    int32_t i;
    for (i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
    return 0;
}


/* dechar
 * DESCRIPTION: type casts a string to an uint8_t array
 * INPUTS: s -- array of chars
 * OUTPUT: none
 * RETURNS: type casted pointer
 * SIDE EFFECTS: none
 */
uint8_t* dechar(const char* s) {
    return (uint8_t*)s;
}

/* print_buf
 * DESCRIPTION: Prints a buffer, char by char
 * INPUT: buf   -- an array of chars
 *        bytes -- the number of bytes to print
 * OUTPUT: none
 * RETURNS: none
 * SIDE EFFECTS: none
 */
void print_buf(uint8_t* buf, uint32_t bytes)
{
	int i;
	for (i = 0; i < bytes; i++)
	{
		putc(buf[i]);
	}
}
