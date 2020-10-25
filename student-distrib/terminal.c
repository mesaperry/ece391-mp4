/* terminal.c - Terminal Implementation */

#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"

#define KEYBOARD_IRQ 0x01
#define KEYBOARD_PORT 0x60

#define INPUT_CUTOFF 0x3E

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

/* terminal_init
 *
 * Initialize terminal vars
 * Input - None
 * Output - None
 */
void terminal_init(void) {
  /* Clear Screen */
  clear();

  /* Clear Terminal(s) */
  clear_buffer(); // Sets keyboard_buffer and key_index

  /* Turn on Keyboard IRQ */
  enable_irq(1);

  /* Intiialize variables */
  R_shift_check = 0;
  L_shift_check = 0;
  ctrl_check = 0;
  capslock_check = 0;
  alt_check = 0;
  enter_down = 0;
  table_index = 0;

  int x;
  for(x = 0; x < MAX_BUFF_LENGTH; x++){
			key_buffer[x] = '\0';
  }
}

/* terminal_open
 *
 * Initializes 3 terminals and relevant video buffers
 * Input - filename
 * Output - returns 0
 */
int32_t terminal_open(const uint8_t* filename) {
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

  /* Disable terminal */
  disable_irq(1);

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
    case CAPS_LOCK:
    {
      /* if on turn off, and vice versa */
      capslock_check = !capslock_check;
      goto SEND_EOI;
    }
    case L_SHIFT:
    {
      /* set keyboard offset to access SHIFTed keys */
      L_shift_check = 1;
      goto SEND_EOI;
    }
    case L_SHIFT_OFF:
    {
      /* Reset keyboard offset to 0, for unSHIFTed keys */
      L_shift_check = 0;
      goto SEND_EOI;
    }
    case R_SHIFT:
    {
      /* set keyboard offset to access SHIFTed keys */
      R_shift_check = 1;
      goto SEND_EOI;
    }
    case R_SHIFT_OFF:
    {
      /* Reset keyboard offset to 0, for unSHIFTed keys */
      R_shift_check = 0;
      goto SEND_EOI;
    }
    case SPACE:
    {
      /* Print space key and add to buffer */
      putc(' ');
      key_buffer[key_index] = ' ';
      key_index++;
      goto SEND_EOI;
    }
    case TAB:
    {
      /* Print tab key (4 spaces) and add to buffer */
      uint32_t x;
      for(x = 0; x < 4; x++){
        putc(' ');
        key_buffer[key_index] = ' ';
        key_index++;
      }
      goto SEND_EOI;
    }
    case BACK:
    {
      /* If at beginning of terminal, return */
      if (key_index <= 0) {
        return 0;
      }

      /* Print backspace */
    //  print_backspace();

      /* Update keyboard buffer */
      key_index--;
      key_buffer[key_index] = '\0';
      goto SEND_EOI;
    }
    case CTRL:
    {
      /* turn on flag */
      ctrl_check = 1;
      goto SEND_EOI;
    }
    case CTRL_OFF:
    {
      /* turn off flag */
      ctrl_check = 0;
      goto SEND_EOI;
    }
    case ALT:
    {
      /* turn on flag */
      alt_check = 1;
      goto SEND_EOI;
    }
    case ALT_OFF:
    {
      /* turn off flag */
      alt_check = 0;
      goto SEND_EOI;
    }
    case ENTER:
    {
      /* put newline character in buffer */
      key_buffer[key_index] = '\n';

      /* set enter flag */
      enter_down = 1;
      goto SEND_EOI;
    }
    default:
    {
      /* Only check for key-press scancodes, no key-release scancodes */
      if(scancode < INPUT_CUTOFF)
      {
        /* Get index within the keyboard table */
        table_index = scancode - TABLE_OFFSET;

        /* Handle shifts */
        if(L_shift_check || R_shift_check){
          table_index += SHIFT_OFFSET;
        }

        /* Handle capslock */
        if(capslock_check){
          /* Only add if the input scancode is within letter bounds */
          /* Letters Q-P: 0x10-0x19
           * Letters A-L: 0x1E-0x26
           * Letters Z-M: 0x2C-0x32 */
          if((scancode >= 0x10 && scancode <= 0x19) || (scancode >= 0x1e && scancode <= 0x26) || (scancode >= 0x2C && scancode <= 0x32)){
            /* Cancel out effect of capslock with shift */
            if(L_shift_check || R_shift_check){
              table_index -= SHIFT_OFFSET;
            }
            else{
              /* Shift letters */
              table_index += SHIFT_OFFSET;
            }
          }
        }

        /* Prep letter value to write */
        input = KEY_TABLE[table_index];

        /* Print letter to screen */
        putc(input);

        /* Load keyboard buffer with symbol */
        key_buffer[key_index] = input;

        /* Update index in keyboard buffer */
        key_index++;
      }
    }
  }

  // Shift line for every 80 presses in key_buff

  SEND_EOI:
      /* Send interrupt signal for keyboard, the first IRQ */
      send_eoi(1);
      return 0;
}
