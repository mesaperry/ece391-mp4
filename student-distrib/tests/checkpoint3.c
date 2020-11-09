#include "../tests.h"
#include "checkpoint2.h"
#include "../types.h"
#include "../terminal.h"
#include "../lib.h"
#include "../filesys.h"
#include "../utils/char_util.h"
#include "../utils/clock_util.h"
#include "../utils/file_util.h"
#include "../rtc.h"
#include "../idt.h"
#include "../syscalls.h"

/* Checkpoint 3 Tests */

int test_linkage()
{
    TEST_HEADER;
    // asm("movl $1, %%eax");
    // asm("int $0x80");
    return PASS;
}




void test_all_checkpoint3()
{
    TEST_OUTPUT("testing idt syscall", test_linkage());
    return;
}
