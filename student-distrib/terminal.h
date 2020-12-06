/* terminal.h - Terminal Implementation */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#define MAX_BUFF_LENGTH 128

#define SHIFT_OFFSET 52
#define TABLE_OFFSET 2
#define KEY_SIZE 104

#define BACK 0x0E
#define BACK_OFF 0x8E
#define TAB 0x0F
#define ENTER 0x1C
#define ENTER_OFF 0x9C
#define SPACE 0x39
#define CAPS_LOCK 0x3A

#define L_SHIFT 0x2A
#define L_SHIFT_OFF 0xAA
#define R_SHIFT 0x36
#define R_SHIFT_OFF 0xB6
#define CTRL 0x1D
#define CTRL_OFF 0x9D
#define ALT 0x38
#define ALT_OFF 0xB8
#define ESC 0x01
#define BACKQUOTE 0x29
#define MAX_TERMINAL_NUM 3
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D

#define MAX_DEVICES 		6

#include "types.h"
#include "lib.h"
#include "i8259.h"

uint8_t R_shift_check;
uint8_t L_shift_check;
uint8_t ctrl_check;
uint8_t capslock_check;
uint8_t alt_check;
uint8_t enter_down;
uint8_t overflow_check;
uint8_t alt_check;

uint8_t shell_check;

uint8_t current_line;
uint8_t current_terminal;
uint8_t running_terminal;  // The current terminal which is running
uint8_t wrapped;
uint8_t clear_offset[MAX_TERMINAL_NUM]; // Used as the key_buffer offset when clearing the screen

uint32_t table_index;
uint32_t key_index[MAX_TERMINAL_NUM];
uint32_t key_offset;
uint8_t key_buffer[MAX_TERMINAL_NUM][MAX_BUFF_LENGTH];
int32_t term_procs[MAX_DEVICES]; // Handles the maximum number of devices the system should run
int32_t running_procs[MAX_TERMINAL_NUM];  // The foremost processes in each terminal

void terminal_init(void);
void set_term_process(int32_t pid);
void remove_term_process(int32_t pid);
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t clear_buffer();
uint32_t get_key_index(void);
uint32_t get_current_terminal(void);
int32_t keyboard_handler(void);

#endif  /* _TERMINAL_H   */
