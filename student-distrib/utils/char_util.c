#include "char_util.h"
#include "../lib.h"

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

int32_t string_equal(const uint8_t* s1, const uint8_t* s2)
{
    if ((s1 == NULL) || (s2 == NULL)) return -1;

    int i;
    for (i = 0; (s1[i] != '\0') && (s2[i] != '\0'); i++) {
        if (s1[i] != s2[i]) return 0;
    }
    if ((s1[i] == '\0') && (s2[i] == '\0')) {
        return 1;
    }
    return 0;
}

int32_t substring(const uint8_t* s, uint8_t* buf, uint32_t start, uint32_t end)
{
    if ((s == NULL) || (buf == NULL)) return -1;
    int32_t len = string_length(s);
    if ((start < 0) || (end < start) || (end > len)) return -1;

    int i;
    for (i = start; i < end; i++) {
        buf[i - start] = s[i];
    }
    buf[end - start] = '\0';
    return 0;
}

void print_buf(uint8_t* buf, uint32_t bytes)
{
	int i;
	for (i = 0; i < bytes; i++)
	{
		putc(buf[i]);
	}
}
