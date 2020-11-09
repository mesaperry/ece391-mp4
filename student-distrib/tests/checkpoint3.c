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

/* test_page_mapping
 * DESCRIPTION: test the function map_v_p for mapping and unmapping pages from
 *                virtuall memory to physical memory
 * INPUTS: none
 * OUTPUTS: PASS/FAIL
 * SIDE EFFECTS: uses 0x5000000 (80MB) and 0x6000000 (96MB) memory locations
 * Coverage: map_v_p
 * Files: paging.h/c
 */
int test_page_mapping() {
    TEST_HEADER;

    uint32_t virt = 0x5000000;
    uint32_t phys = 0x6000000;

    if (map_v_p(0x1234567, virt, 0) != -1) {
        printf("Page mapping should fail when mapping unaligned pages in virtual memory\n");
        return FAIL;
    }
    if (map_v_p(virt, 0x123456, 1) != -1) {
        printf("Page mapping should fail when mapping unaligned pages in virtual memory\n");
        return FAIL;
    }
    if ((map_v_p(virt, phys, 2) != -1) ||
        (map_v_p(virt, -1, 1) != -1) ||
        (map_v_p(-1, virt, 1) != -1)) {
        printf("Page mapping should fail with garbage inputs\n");
        return FAIL;
    }

    /* 4 MB Pages */
    if (map_v_p(virt, phys, 1) != 0) {
        printf("Page mapping %x to %x failed (4MB)\n", virt, phys);
        return FAIL;
    }
    printf("These should not page fault (4MB)...\n");
    *((char*)virt) = 'P';
    printf("First one good.\n");
    *((char*)(virt + MB_4 - 1)) = 'S';
    printf("Last one good.\n");
    printf("disabling paging to test physical address\n");
    disable_paging();
    if ((*(char*)phys != 'P') || (*(char*)(phys + MB_4 - 1) != 'S')) {
        printf("Page mapping does not store data in the right physical address (4MB)\n");
        enable_paging();      // enable again for unmapping
        map_v_p(virt, 0, 1);  // unmap
        return FAIL;
    }
    printf("enabling paging\n");
    enable_paging();
    if (map_v_p(virt, 0, 1) != 0) {
        printf("Page unmapping %x to %x failed (4MB)\n", virt, phys);
        return FAIL;
    }

    /* 8 KB Pages */
    if (map_v_p(virt, phys, 0) != -1) {
        printf("Page mapping 4KB %x to %x succeeded above kernel memory start address \n", virt, phys);
        return FAIL;
    }
    virt = 0xA0000;
    if (map_v_p(virt, phys, 0) != 0) {
        printf("Page mapping %x to %x failed (4KB)\n", virt, phys);
        return FAIL;
    }
    printf("These should not page fault (4KB)...\n");
    *((char*)virt) = '1';
    printf("First one good.\n");
    *((char*)(virt + KB_8/2 - 1)) = '2';
    printf("Last one good.\n");
    printf("disabling paging to test physical address\n");
    disable_paging();
    if ((*(char*)phys != '1') || (*(char*)(phys + KB_8/2 - 1) != '2')) {
        printf("Page mapping does not store data in the right physical address (4KB)\n");
        enable_paging();      // enable again for unmapping
        map_v_p(virt, 0, 0);  // unmap
        return FAIL;
    }
    printf("enabling paging\n");
    enable_paging();
    if (map_v_p(virt, 0, 0) != 0) {
        printf("Page unmapping %x to %x failed (4KB)\n", virt, phys);
        return FAIL;
    }
    printf("PASS if page fault...\n");
    printf("Didn't page fault! Instead, found %s", *((char*)virt));
    return FAIL;
}



void test_all_checkpoint3()
{
    // TEST_OUTPUT("testing idt syscall", test_linkage());
    printf("test");
    clear();
    int32_t f = open_syscall_test();
    printf("\nfd: %d\n\n", f);
    read_syscall_test(f);
    TEST_OUTPUT("Write syscall", write_syscall_test(f));
    TEST_OUTPUT("Close syscall", close_syscall_test(f));
    return;
}
