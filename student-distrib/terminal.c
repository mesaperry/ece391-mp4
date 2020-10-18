/* terminal.c - Terminal Implementation */

#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"

#define KEYBOARD_IRQ 0x01
#define KEYBOARD_PORT 0x60

const unsigned char KEY_TABLE[KEY_SIZE] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9','0','-', '=',' ', ' ',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',']', ' ', ' ',
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', ' ', '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    '!', '@', '#', '$', '%', '^', '&', '*', '(',')', '_', '+', ' ', ' ',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', ' ', ' ',
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', ' ', '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?'
};

/* terminal_open
 *
 * Initialize terminal variables
 * Input - filename
 * Output - Return 0
 */
int32_t terminal_open(const uint8_t* filename)
{
  /* Clear Terminal(s) */
  clear_buffer(); // Sets keyboard_buffer and key_index

  /* Turn on Keyboard IRQ */
  // enable_irq(1);

  /* Intiialize variables */
  enter_down = 0;
  table_index = 0;

  return 0;

}

/* terminal_close
 *
 * Initialize terminal variables
 * Input - fd 0 filediscriptor
 * Output - None
 */
int32_t terminal_close(int32_t fd)
{
  /* Check if fd is input or ouput - shouldn't be closed */
  if(fd == 0 || fd == 1)
  {
    return -1; /* Return fail if I/O fd */
  }

  return 0; /* Success */
}

/* terminal_read
 *
 * read data from keyboard, return number of bytes read
 * Input - fd - file descriptor
 *         buf - buffer
 *         nbytes - number of bytes
 * Output - returns nbytes written on success, -1 on failure
 */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
{
  /* Returns data from one line that has been terminated by pressing
  enter, or as much as fits in the buffer from one such line. Line returned
  should include the line feed character.*/
  /* Initialize local variables */
  uint32_t x, count, index, last;

  /* Loop until enter is pressed */
  enter_down = 0;
  while(!enter_down);
  enter_down = 0;

  /* Fail if no bytes, or negative bytes to be returned */
  if(nbytes <= 0)
  {
    return -1;
  }

  /* Prep byte counter */
  count = 0;

  /* Initialize buffer */
  int8_t* buffer = (int8_t *)buf;

  /* Start critical read section */
  cli();
  /* Load keyboard_buffer into buf, up to MAX_BUFF_LENGTH */
  for(x = 0; x < MAX_BUFF_LENGTH; x++)
  {
    buffer[x] = key_buffer[x];
    last = x + 1;
    count++;
    index = x;

    /* Break if new line symbol is reached */
    if(buffer[x] == '\n')
    {
      break;
    }
  }

  /* Handle cases where buffer's filled before adding newline symbol */
  if(buffer[index] != '\n' && last < MAX_BUFF_LENGTH)
  {
    buffer[last] = '\n'; /* Append newline to end of buffer, if space */
  }
  else if(buffer[index] != '\n')
  {
    buffer[index] = '\n'; /* Append newline to last space in buffer if overflow */
  }

  /* Close critical section */
  sti();

  /* Clear buffer */
  clear_buffer();

  /* Return the number of bytes written */
  return count;

}

/* terminal_write
 *
 * Write all data in buf to the screen
 *
 * Input - fd - file descriptor
 *         buf - buffer
 *         nbytes - number of bytes
 * Output - returns nbytes written on success, -1 on failure
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
{
  int x, count;

  /* Return failure if buf is empty, or no/negative bytes to be written */
  if(buf == NULL || nbytes <= 0)
  {
    return -1;
  }

  /* Initialize buffer */
  int8_t* buffer = (int8_t *)buf;

  /* Open critical section */
  cli();

  /* Write data to the screen */
  for(x = 0; x < nbytes; x++)
  {
    putc(buffer[x]);
    count++;
  }

  /* Close critical section */
  sti();

  /* Return number of bytes written */
  return count;
}

/* clear_buffer
 *
 * Resets keyboard buffers
 * Input - None
 * Output - Returns 0 and empty buffer
 */
int32_t clear_buffer()
{
  /* Initialize variables */
  int x;

  /* Open critical section */
  cli();

  /* Reset key buffer with NULL values */
  for(x = 0; x < MAX_BUFF_LENGTH; x++)
  {
    key_buffer[x] = '\0';
  }

  key_index = 0; /* Reset keyboard buffer index */

  /* Close critical section */
  sti();

  /* Return 0 on success */
  return 0;
}

/*
* keyboard_handler(void)
* DESCRIPTION: Works with the assemlby linkage to fill keyboard buffer
* INPUTS: None
* OUTPUT: None
*/
int32_t keyboard_handler(void)
{
  /* Initialize variables */
  uint8_t scancode, input;

  /* Get keystroke from keyboard */
  scancode = inb(KEYBOARD_PORT);

  /* Handle keystroke */
  switch(scancode)
  {
    /* Get index within the keyboard table */
    table_index = scancode - TABLE_OFFSET;

    /* Prep letter value to write */
    input = KEY_TABLE[table_index];

    /* Open critical section */
    cli();

    /* Print letter to screen */
    putc(input);

    /* Load keyboard buffer with symbol */
    key_buffer[key_index] = input;

    /* Update index in keyboard buffer */
    key_index++;

    /* Close critical section */
    sti();

  }

  /* Send interrupt signal for keyboard, the first IRQ */
  //send_eoi(1);
  return 0;
}
