#include "../tests.h"
#include "checkpoint1.h"
#include "../types.h"

#include "../idt.h"
#include "../paging.h"
#include "../lib.h"
#include "../terminal.h"

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
	int8_t test_buffer[nbytes];
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
	int x = 128;
	uint8_t buffer[MAX_BUFF_LENGTH];
	while(1){
		x = terminal_read(1, buffer, 128);
		terminal_write(1, buffer, 128);
	}
	return 1;
}

/* Page test
 *
 * Dereferences valid locations
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: dereference
 * Files: paging.h/c
 */
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

void test_all_checkpoint1()
{
    // TEST_OUTPUT("idt_test", idt_test());
    // TEST_OUTPUT("page_test", page_test());
    TEST_OUTPUT("keyboard", keyboard_test());

    /* What does this do? */
    // TEST_OUTPUT("IDT without paging", idt_woP());

    /* Tests intended to fail  */
    // TEST_OUTPUT("bounds range", bound_range());
    // TEST_OUTPUT("divide by zero", divide_0());
    // TEST_OUTPUT("dereference null", null_deref());
    // TEST_OUTPUT("Force page exception 1", force_page_exception_1());
    // TEST_OUTPUT("Force page exception 2", force_page_exception_2());
}
