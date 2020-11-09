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

/* Checkpoint 2 tests */


/* print_all_filesys
 *
 * Print out the filesys like ls would in linux
 * Inputs: None
 * Outputs: none
 * Side Effects: prints the filesystem to the terminal
 * Coverage: directory_read
 * Files: filesys.h/c
 */
int print_all_filesys()
{
	TEST_HEADER;
	uint8_t name[FNAME_MAX_LEN + 1];
	while (directory_read(name)) print_file_info(name);
	return PASS;
}

/* read_data_filesys
 *
 * Demonstrates read_data reading bytes
 * Inputs: None
 * Outputs: none
 * Side Effects: prints the inodes checked before it found the data and how many bytes it compied
 * Coverage: read_data
 * Files: filesys.h/c
 */
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


/* filesys_corner_cases
 *
 * Attempts to read invalid dentrys
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: read_dentry_by_name/by_index
 * Files: filesys.h/c
 */
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


/* test_read_file_bytes_by_name
 *
 * reads the begining of files and makes sure that each file is appropriate type
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: read_dentry_by_name/by_index/read_data
 * Files: filesys.h/c
 */
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


/* reading_help
 *
 * Helper function that performs the actual test for read_files
 * gets the file size and creates appropriate buffer
 * then reads the data from the file to the screen and waits 5000ms
 * Inputs: fname 	- 	the filename to read
 * Outputs: none
 * Side Effects: the file is read to the terminal screen
 */
void reading_help(char* fname)
{
	uint32_t size;
	size = file_size((uint8_t*)fname);
	uint8_t data[size];
	printf("%s \n", fname);
	read_file_bytes_by_name((uint8_t*)fname, data, size);
	print_buf(data, size);
	printf("\n");

	wait_sync(2000);
}

/* test_read_files
 *
 * reads example files for tests
 * Inputs: None
 * Outputs: none
 * Side Effects: reads sample files to the terminal screen
 * Coverage: read_data/read_dentry_by_name
 * Files: filesys.h/c
 */
int test_read_files()
{
	TEST_HEADER;


	reading_help("frame0.txt");

	reading_help("frame1.txt");

	reading_help("fish");

	reading_help("verylargetextwithverylongname.txt");

	reading_help("grep");

	reading_help("ls");


	printf("\n");
	return PASS;
}

/* test_open_close
 *
 * attempts to open files that dont exist and close files
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: file_open/file_close/dir_open/dir_close
 * Files: filesys.h/c
 */
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

/* test_write
 *
 * attempts to write to files - nothing should happen read-only
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: file_write/dir_write
 * Files: filesys.h/c
 */
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

	return PASS;
}

void test_all_checkpoint2()
{
	TEST_OUTPUT("print all filesys", print_all_filesys());
	wait_sync(2000);
	TEST_OUTPUT("read filesys inode", read_data_filesys());
	wait_sync(2000);
	TEST_OUTPUT("filesys_corner_cases", filesys_corner_cases());
	wait_sync(2000);
	TEST_OUTPUT("test read file bytes by name", test_read_file_bytes_by_name());
	wait_sync(2000);
	TEST_OUTPUT("Read Files Test: ", test_read_files());
	wait_sync(2000);
	TEST_OUTPUT("Test Open and Close", test_open_close());
	wait_sync(2000);
	TEST_OUTPUT("Test writing files", test_write());
	wait_sync(2000);
	TEST_OUTPUT("rtc driver test", rtc_driver_test());
	wait_sync(2000);
	TEST_OUTPUT("rtc rate test", rtc_rate_test());

	printf("Checkpoint 2 Tests Done.\n");
}
