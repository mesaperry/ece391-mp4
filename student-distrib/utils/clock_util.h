/* Clock util file, applying rtc functions */
#include "../types.h"

/* wait a certain amount of time in milliseconds, synchronous with thread */
void wait_sync(uint32_t ms_delay);

/* wait a certain amount of time in milliseconds, asynchronous with thread */
void wait_async(uint32_t ms_delay);
