#include "../tests.h"
#include "checkpoint2.h"
#include "../types.h"
#include "../terminal.h"
#include "../lib.h"
#include "../filesys.h"
#include "../utils/char_util.h"
#include "../utils/clock_util.h"
#include "../rtc.h"

/* Checkpoint 2 tests */

int print_all_filesys()
{
	TEST_HEADER;
	uint8_t name[FNAME_MAX_LEN + 1];
	while (directory_read(name)) {
		printf("%s", name);
		printf("\n");
	}
	return PASS;
}

int read_data_filesys()
{
	TEST_HEADER;
	uint8_t buf[80];
	uint32_t inode_index;
	int32_t bytes_read;
	for (inode_index = 0; (bytes_read = read_data(inode_index, 0, buf, 80)) == 0; inode_index++) {
		if (bytes_read < 0) return FAIL;
	}
	printf("%d inodes checked before data found, %d bytes copied:\n", inode_index, bytes_read);
	// int b;
	// for (b = 0; b < bytes_read; b++) printf("%c", buf[b]);
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

/* Retuns 0 if file read correctly, -1 if failed */
int read_file_bytes_by_name(uint8_t* fname, uint8_t* buf, uint32_t length)
{
	dentry_t dentry;
	if (read_dentry_by_name(fname, &dentry) < 0) return -1;
	if (read_data(dentry.inode_index, 0, buf, length) < 0) return -1;
	return 0;
}

/* Returns -1 if not executable, 0 if executable */
int verify_executable(char* exe)
{
	uint8_t data[11];
	data[10] = '\0';
	if (read_file_bytes_by_name((uint8_t*)exe, data, 10) < 0) return -1;
	uint8_t buf[4];
	substring(data, buf, 1, 4);
	if (string_equal(buf, (uint8_t*)"ELF") == 0) return -1;
	return 0;
}

int test_read_file_bytes_by_name()
{
	TEST_HEADER;
	uint8_t data[11];
	data[10] = '\0';
	if (read_file_bytes_by_name((uint8_t*)"frame0.txt", data, 10) < 0) return FAIL;
	if (!string_equal(data, (uint8_t*)"/\\/\\/\\/\\/\\")) {
		printf("did not read fram0.txt correctly\n");
		printf("data: %s\n", data);
		printf("file: /\\/\\/\\/\\/\\\n");
		return FAIL;
	}
	if (verify_executable("ls") < 0) {
		printf("ls not executable\n");
		return FAIL;
	}
	if (verify_executable("grep") < 0) {
		printf("grep not executable\n");
		return FAIL;
	}
	if (verify_executable("frame0.txt") == 0) {
		printf("frame is executable\n");
		return FAIL;
	}
	if (verify_executable("pieter") == 0) {
		printf("pieter exists and is executable\n");
		return FAIL;
	}
	return PASS;
}


// test read files
int test_read_files()
{
	TEST_HEADER;
	uint8_t data[10000];
	char read[10];

	printf("frame0.txt:\n");
	read_file_bytes_by_name((uint8_t*)"frame0.txt", data, 50);
	print_buf(data, 50);

	printf("\n");

	while (1)
	{
		
		if (terminal_read(1, read, 1)) break;
	}

	printf("frame1.txt:\n");
	if (read_file_bytes_by_name((uint8_t*)"frame1.txt", data, 500) != -1)
	{
		print_buf(data, 500);
	}


	printf("\n\n");

	while (1)
	{
		
		if (terminal_read(1, read, 1)) break;
	}

	printf("fish:\n");
	if (read_file_bytes_by_name((uint8_t*)"fish", data, 4500) != -1)
	{

		print_buf(data, 4500);
	}

	printf("\n");

	while (1)
	{
		
		if (terminal_read(1, read, 1)) break;
	}

	printf("verlargetextwithverylongname.tx(t):\n");
	if (read_file_bytes_by_name((uint8_t*)"verylargetextwithverylongname.txt", data, 10000) != -1) print_buf(data, 10000);

	printf("\n");

	while (1)
	{
		
		if (terminal_read(1, read, 1)) break;
	}

	printf("grep:\n");
	if (read_file_bytes_by_name((uint8_t*)"grep", data, 5000) != -1) print_buf(data, 5000);

	printf("\n");

	while (1)
	{
		
		if (terminal_read(1, read, 1)) break;
	}

	printf("ls:\n");
	if (read_file_bytes_by_name((uint8_t*)"ls", data, 5000) != -1) print_buf(data, 5000);

	printf("\n");
	return PASS;
}

// test opens_and_closes
int test_open_close()
{
	TEST_HEADER;
	dentry_t dentry;
	if (read_dentry_by_name((uint8_t*)"hag_in_a_bag.txt", &dentry) != -1)
	{
		printf("lol open_file failure\n");
		return FAIL;
	}
	printf("hag_in_a_bag.txt doesn't exist : good \n");

	if (read_dentry_by_name((uint8_t*)"frame0.txt", &dentry) == -1)
	{
		printf("No you're wrong its there, I saw it in air bud\n");
		return FAIL;
	}
	printf("frame0.txt exisits\n");

	if (file_open((uint8_t*)"frame0.txt") == -1)
	{
		printf("file not opened\n");
		return FAIL;
	}
// always ret 1
	if (dir_open((uint8_t*)"") == -1)
	{
		printf("dir not opend");
		return FAIL;
	}

	if (file_close((uint8_t*)"frame0.txt") == -1)
	{
		printf("cannot close file\n");
		return FAIL;
	}

	if (dir_close((uint8_t*)"") == -1)
	{
		printf("literally how did you mess this up?");
		return FAIL;
	}

	return PASS;
}

int test_write()
{
	TEST_HEADER;
	uint8_t buf[100];
	buf[99] = '\0';
	if (file_write((uint8_t*)"frame0.txt", buf, 20) != -1)
	{
		printf("ERROR: shudnt be able to write\n");
		return FAIL;
	}
	printf("Cannot write Read-Only\n");

	if (dir_write((uint8_t*)"", buf, 20) != -1)
	{
		printf("ERROR: shudnt be able to write\n");
		return FAIL;
	}
	printf("Cannot write Read-Only\n");
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
 * Outputs: PASS
 * Side Effects: Tries to print to console every second, regardless of RTC rate
 * Coverage: rtc_write
 * Files: rtc.h/c
 */
int rtc_rate_test()
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

	return PASS
}

void test_all_checkpoint2()
{
	TEST_OUTPUT("print all filesys", print_all_filesys());
	TEST_OUTPUT("read filesys inode", read_data_filesys());
	TEST_OUTPUT("filesys_corner_cases", filesys_corner_cases());
	TEST_OUTPUT("test read file bytes by name", test_read_file_bytes_by_name());
	TEST_OUTPUT("Read Files Test: ", test_read_files());
	TEST_OUTPUT("Open close", test_open_close());
	TEST_OUTPUT("Write Test", test_write());
	TEST_OUTPUT("RTC Driver Input/Return", rtc_driver_test());
	TEST_OUTPUT("RTC Rates", rtc_rate_test());
}
