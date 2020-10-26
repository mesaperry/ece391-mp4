#include "clock_util.h"
#include "../rtc.h"

/*
 * wait_sync
 * DESCRIPTION: waits a certain amount of time in milliseconds, synchronously
 * INPUTS: time in milliseconds
 * OUTPUTS: none
 * RETURNS: none
 * SIDE EFFECTS: none
 */
void wait_sync(uint32_t ms_delay)
{
    const uint32_t frequency = 512;         // current frequency in Hz
    int num_ticks;                          // number of RTC ticks to wait for
    int tick;                               // current tick

    rtc_open(NULL);
    rtc_write(0, &frequency, sizeof(frequency));

    num_ticks = (ms_delay * frequency) / 1000;

    /* read <num_ticks> ticks before returning */
    for (tick = 0; tick < num_ticks; tick++) {
        rtc_read(0, NULL, 0);
    }

}

/*
 * wait_async
 * DESCRIPTION: waits a certain amount of time in milliseconds, asynchronously
 * INPUTS: time in milliseconds
 * OUTPUTS: none
 * RETURNS: none
 * SIDE EFFECTS: none
 */
void wait_async(uint32_t ms_delay)
{
    // Cannot yet be implemented
}
