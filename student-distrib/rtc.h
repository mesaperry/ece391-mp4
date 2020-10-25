/* rtc.h - real time clock implementation
 * vim:ts=4 noexpandtab
 */

#include "types.h"

void rtc_init();
void rtc_handler();
void rtc_open();
int32_t rtc_close();
