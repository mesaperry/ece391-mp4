#include "tests.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"
#include "terminal.h"
#include "filesys.h"
#include "utils/char_util.h"

#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $12");
}


/* Checkpoint 1 tests */

/* IDT Test - whole table
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 256; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* Divide by Zero Exception Test
 *
 * Asserts divide by 0 Exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT
 * Files: idt.c
 */
int divide_0(){
	int y = 0;
	int x = 12;
	x = x/y;
	//assertion_failure();
	return FAIL;
}

/* dereferencing null Exception Test
 *
 * Asserts Exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT
 * Files: idt.c
 */
int null_deref(){
	int* y;
	int x;
	y = 0;
	x = *(y);
	//assertion_failure();
	return FAIL;
}

/* bounds Exception Test
 *
 * Asserts bound range Exception
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT
 * Files: idt.c
 */
int bound_range(){
	int y[2];
	int x;
	x = y[3];
	//assertion_failure();
	return FAIL;
}

int idt_woP(){
	assertion_failure();
	printf("1");
	return 1;
}

/* term_write_test
 *
 * Print out characters passed in by buffer
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: terminal_write
 * Files: terminal.h/c
 */
void term_write_test(){
	uint32_t nbytes = 4;
	uint8_t test_buffer[nbytes];
	test_buffer[0] = 'a';
	test_buffer[1] = 'b';
	test_buffer[2] = 'c';
	test_buffer[3] = 'd';

	printf ("Input: abcd\n");
	printf("Terminal prints: ");
	uint32_t bytes_read = terminal_write(0, test_buffer, nbytes);

	printf("\nNumber of bytes written: %d", bytes_read);

}

/* Keyboard test
 *
 * Checks if the keyboard functions
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read, terminal_write
 * Files: terminal.h/c
 */
int keyboard_test(){
	int x = 0;
	uint8_t buffer[MAX_BUFF_LENGTH];

	while(1){
		x = terminal_read(1, buffer, x);
		terminal_write(1, buffer, x);
	}
	return 1;
}

int page_test()
{
	TEST_HEADER;
	char _ignore0 __attribute__((unused)) = *(char*)VIDEO;
	char _ignore1 __attribute__((unused)) = *(char*)KERNEL_MEMORY_ADDR;
	return PASS;
}

/* These should fail */
int force_page_exception_1()
{
	TEST_HEADER;
	char _ignore __attribute__((unused)) = *(char*)(0x0);
	return FAIL;
}

int force_page_exception_2()
{
	TEST_HEADER;
	char _ignore __attribute__((unused)) = *(char*)(0xFFFFFFFF);
	return FAIL;
}

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
	int count = 0;
	
	printf("frame0.txt:\n");
	read_file_bytes_by_name((uint8_t*)"frame0.txt", data, 50);
	print_buf(data, 50);
<<<<<<< HEAD

	printf("\n");

	printf("frame1.txt:\n");
=======
	

	printf("\n");
	

	printf("frame1.txt:\n");	
>>>>>>> c3304160540393a58afa91f64c6ce5656bd2ece1
	if (read_file_bytes_by_name((uint8_t*)"frame1.txt", data, 500) != -1)
	{
		print_buf(data, 500);
	}


	printf("\n\n");
<<<<<<< HEAD
=======
	
>>>>>>> c3304160540393a58afa91f64c6ce5656bd2ece1

	printf("fish:\n");
	if (read_file_bytes_by_name((uint8_t*)"fish", data, 4500) != -1)
	{

		print_buf(data, 4500);
	}
<<<<<<< HEAD

	printf("\n");
=======
	
	
	printf("\n");
	
>>>>>>> c3304160540393a58afa91f64c6ce5656bd2ece1

	printf("verlargetextwithverylongname.tx(t):\n");
	if (read_file_bytes_by_name((uint8_t*)"verylargetextwithverylongname.txt", data, 10000) != -1) print_buf(data, 10000);


	printf("\n");
	
	
	printf("grep:\n");
<<<<<<< HEAD

	if (read_file_bytes_by_name((uint8_t*)"grep", data, 5000) != -1) print_buf(data, 5000);

=======
	if (read_file_bytes_by_name((uint8_t*)"grep", data, 5000) != -1) print_buf(data, 5000); 
	
>>>>>>> c3304160540393a58afa91f64c6ce5656bd2ece1

	printf("\n");
	

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

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("Deref video and kernel memory", page_test());
	// TEST_OUTPUT("Force page exception 1", force_page_exception_1());
	// TEST_OUTPUT("Force page exception 2", force_page_exception_2());
	//TEST_OUTPUT("divide by zero", divide_0());
	//TEST_OUTPUT("dereference null", null_deref());
	//TEST_OUTPUT("bounds range", bound_range());
	//TEST_OUTPUT("pf", pf());
	// launch your tests here
	//TEST_OUTPUT("IDT without paging", idt_woP());
	//TEST_OUTPUT("keybord", );
	// keyboard_test();

	/* start checkpoint 2*/
	// TEST_OUTPUT("print all filesys", print_all_filesys());
	// TEST_OUTPUT("read filesys inode", read_data_filesys());
	// TEST_OUTPUT("filesys corner cases", filesys_corner_cases());
	//TEST_OUTPUT("test read file bytes by name", test_read_file_bytes_by_name());
	// TEST_OUTPUT("Read Files Test: ", test_read_files());
	//TEST_OUTPUT("Open close", test_open_close());
	//TEST_OUTPUT("Write Test", test_write());
}
