#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"

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
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
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
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

/* term_write_test
 *
 * Print out characters passed in by buffer
 * Inputs: None
 * Outputs: output string
 * Side Effects: None
 * Coverage: terminal_write
 * Files: terminal.h/c
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

} */

/* Keyboard test
 *
 * Checks if the keyboard functions
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: terminal_read, terminal_write
 * Files: keyboard.h/c, terminal.h/c
void keyboard_test(){
	int x = 0;
	uint8_t buffer[MAX_BUFFER_LENGTH];

	terminal_open(NULL);

	while(1){
		x = terminal_read(1, buffer, x);
		terminal_write(1, buffer, x);
	}
} */

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
}
