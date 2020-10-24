#include "tests.h"
#include "x86_desc.h"
#include "paging.h"
#include "lib.h"
#include "terminal.h"
#include "filesys.h"

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

	terminal_init();

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
	char* name;
	while ((name = directory_read()) != NULL) {
		printf(name);
		printf("\n");
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
	if (read_dentry_by_name("nonexistent.file", &dentry) != -1) {
		printf("Did not return failure on reading nonexistent file name\n");
		return FAIL;
	}
	if (read_dentry_by_name("frame0.txt", &dentry) != 0) {
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
<<<<<<< HEAD
	//keyboard_test();
=======
	// keyboard_test();
	// TEST_OUTPUT("print all filesys", print_all_filesys());
	// TEST_OUTPUT("read filesys inode", read_data_filesys());
	// TEST_OUTPUT("filesys corner cases", filesys_corner_cases());
>>>>>>> 067e9235574c23cf3d71c2b72e63372dfa38a773
}
