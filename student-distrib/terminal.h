/* terminal.h - Terminal Implementation */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define MAX_BUFF_LENGTH 128

/* Taken from lib.c */
#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x7

#define TABLE_OFFSET 2
#define KEY_SIZE 104

#include "types.h"
#include "lib.h"
#include "i8259.h"

uint8_t enter_down;
uint32_t table_index;
uint32_t key_index;
uint8_t key_buffer[MAX_BUFF_LENGTH];

void terminal_init(void);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t clear_buffer();
int32_t keyboard_handler(void);

#endif  /* _TERMINAL_H   */
