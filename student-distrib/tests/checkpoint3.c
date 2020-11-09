#include "../tests.h"
// #include "checkpoint2.h"
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

/* System call test (read)
 *
 * Prints out the name of the file in the directory
 * Inputs: None
 * Outputs: filename
 * Side Effects: None
 * Coverage: read
 * Files: sys_calls.h/c
 */
void read_syscall_test(int32_t fd) {
	TEST_HEADER;

	uint32_t i;
	uint8_t buffer[FNAME_MAX_LEN];
	uint32_t bytes_read = read(fd, buffer, FNAME_MAX_LEN);
	if (bytes_read == -1 || bytes_read == 0) {
		printf("FAIL: No files found\n");
	} else {
		printf("File size: %d bytes\n", bytes_read);
		for (i = 0; i < bytes_read; i++) {
			putc(buffer[i]);
		}
	}
}

/* System call test (write)
 *
 * Tries writing to file
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: write
 * Files: sys_calls.h/c
 */
int write_syscall_test(int32_t fd) {
	TEST_HEADER;

	uint8_t buffer[FNAME_MAX_LEN];
	uint32_t bytes_read = write(fd, buffer, FNAME_MAX_LEN);
	if (bytes_read == -1) {
		return PASS;
	} else {
		return FAIL;
	}
}

/* System call test (open)
 *
 * Opens a file
 * Inputs: None
 * Outputs: fd
 * Side Effects: None
 * Coverage: open
 * Files: sys_calls.h/c
 */
int32_t open_syscall_test() {
	TEST_HEADER;

	int32_t fd = open((uint8_t*)("frame0.txt"));
	if (fd == -1) {
		printf("FAIL: File not found\n");
	} else {
		printf("PASS: File found\n");
	}
	return fd;
}

/* System call test (close)
 *
 * Closes a file
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: close
 * Files: sys_calls.h/c
 */
int close_syscall_test(int32_t fd) {
	TEST_HEADER;

	uint32_t bytes_read = close(fd);
	if (bytes_read == 0) {
		return PASS;
	} else {
		return FAIL;
	}
}




void test_all_checkpoint3()
{
    // TEST_OUTPUT("testing idt syscall", test_linkage());
    clear();
    int32_t f = open_syscall_test();
    printf("\nfd: %d\n\n", f);
    read_syscall_test(f);
    TEST_OUTPUT("Write syscall", write_syscall_test(f));
    TEST_OUTPUT("Close syscall", close_syscall_test(f));
    return;
}
