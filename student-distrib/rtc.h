/* rtc.h - real time clock implementation
 * vim:ts=4 noexpandtab
 */
#ifndef _RTC_H
#define _RTC_H
#include "types.h"

void rtc_init();
void rtc_handler();

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);

uint8_t rtc_get_rate();
int rtc_is_on();

#endif /* _RTC_H */
