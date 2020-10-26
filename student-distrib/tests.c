#include "tests.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"
#include "terminal.h"
#include "filesys.h"
#include "utils/char_util.h"
#include "tests/checkpoint1.h"

#include "rtc.h"

/* Checkpoint 2 tests */

int print_all_filesys()
{
	TEST_HEADER;
	uint8_t name[FNAME_MAX_LEN + 1];
	directory_read(name);
	while (name != NULL) {
		printf("%s", name);
		printf("\n");
		directory_read(name);
	}
	return PASS;
}

int read_data_filesys()
{
	TEST_HEADER;
	uint8_t buf[80];
	uint32_t inode_index;
	uint32_t bytes_read;
	int b;
	for (inode_index = 0; (bytes_read = read_data(inode_index, 0, buf, 80)) == 0; inode_index++) {
		if (bytes_read < 0) return FAIL;
	}
	printf("%d inodes checked before data found, %d bytes copied:\n", inode_index, bytes_read);
	for (b = 0; b < bytes_read; b++) printf("%c", buf[b]);
	printf("\n");
	return PASS;
}

int filesys_corner_cases()
{
	TEST_HEADER;
	dentry_t dentry;
	if (read_dentry_by_name((uint8_t*)"nonexistent.file", &dentry) != -1) {
		printf("Did not return failure on reading nonexistent file name\n");
		return FAIL;
	}
	if (read_dentry_by_name((uint8_t*)"frame0.txt", &dentry) != 0) {
		printf("Did not return success on reading existing file name\n");
		return FAIL;
	}
	if ((dentry.name[0] != 'f') || (dentry.name[5] != '0')) {
		printf("Did not find correct file when reading file name\n");
		return FAIL;
	}
	if (read_dentry_by_index(99999, &dentry) != -1) {
		printf("Did not return failure on reading nonexistent dentry index\n");
		return FAIL;
	}
	if (read_dentry_by_index(0, &dentry) != 0) {
		printf("Did not return success on reading existing dentry index\n");
		return FAIL;
	}
	return PASS;
}

/* rtc_driver_test
 *
 * Tests bad inputs and return values
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc_read, rtc_write, rtc_open, rtc_close
 * Files: rtc.h/c
 */
int rtc_driver_test()
{
	int32_t freq;
	int16_t freq_16;

	if (rtc_open(NULL) != 0) {
		printf("rtc_open didn't return successfully\n");
		return FAIL;
	}
	if (rtc_read(0, NULL, 0) != 0) {
		printf("rtc_read didn't return successfully\n");
		return FAIL;
	}
	if (rtc_write(0, NULL, 0) != -1) {
		printf("rtc_write didn't reject a NULL input\n");
		return FAIL;
	}
	freq_16 = 32;
	if (rtc_write(0, &freq_16, sizeof(freq_16)) != -1) {
		printf("rtc_write didn't reject a 2 byte input\n");
		return FAIL;
	}
	freq = 515;
	if (rtc_write(0, &freq, sizeof(freq)) != -1) {
		printf("rtc_write didn't reject a non-power-of-2 frequency\n");
		return FAIL;
	}
	freq = 2048;
	if (rtc_write(0, &freq, sizeof(freq)) != -1) {
		printf("rtc_write didn't reject a >1024 Hz frequency\n");
		return FAIL;
	}
	freq = 1;
	if (rtc_write(0, &freq, sizeof(freq)) != -1) {
		printf("rtc_write didn't reject a 1 Hz frequency\n");
		return FAIL;
	}
	freq = 2;
	if (rtc_write(0, &freq, sizeof(freq)) != 0) {
		printf("rtc_write didn't succeed with a 2 Hz frequency\n");
		return FAIL;
	}
	if (rtc_close(0) != 0) {
		printf("rtc_close didn't return successfully\n");
		return FAIL;
	}

	return PASS;
}

/* rtc_rate_test
 *
 * Tests varying RTC frequencies
 * Inputs: None
 * Outputs: None
 * Side Effects: Tries to print to console every second, regardless of RTC rate
 * Coverage: rtc_write
 * Files: rtc.h/c
 */
void rtc_rate_test()
{
	int num_secs = 2; // number of seconds to test each frequency for
	const int32_t test_freqs[] = {
		2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
	};
	const int num_freqs = sizeof(test_freqs) / sizeof(int32_t);
	int32_t freq;
	int freq_i;
	int tick;

	rtc_open(NULL);

	for (freq_i = 0; freq_i < num_freqs; freq_i++) {

		freq = test_freqs[freq_i];
		rtc_write(0, &freq, sizeof(freq));

		for (tick = 0; tick < num_secs * freq; tick++) {

			/* wait for RTC tick */
			if (tick == 0 || (tick % freq == 0)) {
				printf("%dth tick on %d Hz RTC clock\n", tick, freq);
			}
			rtc_read(0, NULL, 0);

		}
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	test_all_checkpoint1();

	/* start checkpoint 2*/
	// TEST_OUTPUT("print all filesys", print_all_filesys());
	// TEST_OUTPUT("read filesys inode", read_data_filesys());
	// TEST_OUTPUT("filesys corner cases", filesys_corner_cases());
	//TEST_OUTPUT("test read file bytes by name", test_read_file_bytes_by_name());
	// TEST_OUTPUT("Read Files Test: ", test_read_files());
	//TEST_OUTPUT("Open close", test_open_close());
	//TEST_OUTPUT("Write Test", test_write());
	// TEST_OUTPUT("RTC Driver Input/Return", rtc_driver_test());
	// rtc_rate_test();
}
