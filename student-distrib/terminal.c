/* terminal.c - Terminal Implementation */

#include "terminal.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "paging.h"

#include "utils/char_util.h"

#define KEYBOARD_IRQ  0x01
#define KEYBOARD_PORT 0x60

#define INPUT_CUTOFF  0x3E
#define TERM_MEM      0xC0000

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

/* get_term_vid_addr
 * Gets the starting address for that terminal's video cache
 */
uint32_t get_term_vid_addr(uint32_t term)
{
    return TERM_MEM + 4096*term;
}

/* terminal_init
 *
 * Initialize terminal vars
 * Input - None
 * Output - None
 */
void terminal_init(void) {
  int x,y;
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
  current_line = 0;
  wrapped = 0;
  clear_offset[0] = 0; // Used as the key_buffer offset when clearing the screen
  clear_offset[1] = 0;
  clear_offset[2] = 0;
  shell_check = 0;
  alt_check = 0;
  current_terminal = 0;

  for(y = 0; y < MAX_TERMINAL_NUM; y++)
  {
    for(x = 0; x < MAX_BUFF_LENGTH; x++)
    {
        key_buffer[y][x] = '\0';
    }
  }

  for(x = 0; x < MAX_DEVICES; x++)
  {
    term_procs[x] = -1;
  }

  /* Set cursor to top-left of screen */
  update_cursor(SHELL_OFFSET,0);

  /* Allocate terminal video memory pages */
  for (x = 0; x < 2; x++) {
    map_v_p(get_term_vid_addr(x), get_term_vid_addr(x), 0, 1, 1);
  }

}

/* set_term_process
 *
 * Associates process id to its terminal number
 * Input - pid
 * Output -
 */
void set_term_process(int32_t pid)
{
  term_procs[pid] = current_terminal; // Set process to hold value of the terminal it's being run in
}

/* remove_term_process
 *
 * Disassociates process id from its terminal number
 * Input - pid
 * Output -
 */
void remove_term_process(int32_t pid)
{
  term_procs[pid] = -1; // Set process to hold value of the terminal it's being run in
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

  /* Initialize temp buffer */
  int8_t buffer[MAX_BUFF_LENGTH];

  update_cursor(get_screen_x(), get_screen_y());

  for(x = 0; x < MAX_BUFF_LENGTH; x++)
  {
    buffer[x] = '\0';
  }

  /* Loop until enter is pressed */

  enter_down = 0;
  sti();
  while(!enter_down);
  cli();
  enter_down = 0;

  /* Fail if no bytes, or negative bytes to be returned */
  if(nbytes <= 0)
  {
    return -1;
  }

  /* Prep byte counter */
  count = 0;


  /* Load keyboard_buffer into buf, up to MAX_BUFF_LENGTH */
  for(x = 0; x < MAX_BUFF_LENGTH; x++)
  {
    buffer[x] = key_buffer[current_terminal][x];
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

  /* Store temp buffer in buf */
  strncpy(buf, buffer, MAX_BUFF_LENGTH);

  /* Clear buffer */
  clear_buffer();

  for(x = 0; x < count; x++)
  {
    buffer[x] = '0';
  }

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
  int32_t x, count;


  /* Initialize temp buffer */
  int8_t buffer[nbytes];

  count = 0;
  for(x = 0; x < nbytes; x++)
  {
    buffer[x] = '\0';
  }

  /* Return failure if buf is empty, or no/negative bytes to be written */
  if(buf == NULL || nbytes <= 0)
  {
    return -1;
  }

  /* Store temp buffer in buf */
  copy_buf((uint8_t*)buf, (uint8_t*)buffer, nbytes);

  /* Write data to the screen */
  for(x = 0; x < nbytes; x++)
  {
    if((x % 80 == 0) && x != 0 && (get_screen_y() + 1 == NUM_ROWS))
    {
        set_screen_y(get_screen_y() + 1);
        scroll_handle();
    } else if ((x % 80 == 0) && (x != 0)) {
        wrap_around();
    }

    if(buffer[x] != '\0')
    {
      putc(buffer[x]);
      count++;
    }
  }

  // /* Close critical section */
  // sti();

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

  // /* Open critical section */
  // cli();

  /* Reset key buffer with NULL values */
  for(x = 0; x < MAX_BUFF_LENGTH; x++)
  {
    key_buffer[current_terminal][x] = '\0';
  }

  key_index[0] = 0; /* Reset keyboard buffer index */
  key_index[1] = 0;
  key_index[2] = 0;

  // /* Close critical section */
  // sti();

  /* Return 0 on success */
  return 0;
}

/* get_key_index
 * used by lib.c
 * Returns the value of the key index
 * Input - None
 * Output - Returns key_index
 */
uint32_t get_key_index(void)
{
  /* Return variable */
  return key_index[current_terminal];
}

/* get_current_terminal
 * used by lib.c
 * Returns the value of the current terminal
 * Input - None
 * Output - Returns current_terminal
 */
uint32_t get_current_terminal(void)
{
  /* Return variable */
  return current_terminal;
}

/* task_switch
 *  DESCRIPTION:
 *  INPUT:
 *  OUTPUT:
 *  RETURNS:
 *  SIDE EFFECTS:
 */
uint32_t term_switch(uint32_t term) {

    uint32_t i;
    uint32_t cur_term = get_current_terminal();
    /* Save state ?? Maybe don't need to */
    // temp_x[term] =
    // temp_y[term] =
    // wrapped[term] =

    cli();

    /* Map virtual video memory to physical video memory */
    map_v_p(VIDEO, VIDEO, 0, 1, 1);

    /* Map current terminal's virtual video memory to its own physical memory */
    map_v_p(get_term_vid_addr(cur_term), get_term_vid_addr(cur_term), 0, 1, 1);

    /* Copy video memory into current process' cold storage */
    for (i = 0; i < 4096; i++) {
        ((uint8_t*) get_term_vid_addr(cur_term))[i] = ((uint8_t*) VIDEO)[i];
    }

    /* Map next terminal's virtual video memory to its own physical memory */
    map_v_p(get_term_vid_addr(term), get_term_vid_addr(term), 0, 1, 1);

    /* Copy video memory into current process' cold storage */
    for (i = 0; i < 4096; i++) {
        ((uint8_t*) VIDEO)[i] = ((uint8_t*) get_term_vid_addr(cur_term))[i];
    }

    sti();
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
  uint8_t scancode, input, temp_x, temp_y, temp;

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
      temp = key_index[current_terminal];

      /* Print space key and add to buffer */
      putc(' ');
      key_buffer[current_terminal][temp] = ' ';
      key_index[current_terminal]++;

      /* Update cursor                                                                */
      update_cursor(get_screen_x(), get_screen_y());

      goto SEND_EOI;
    }
    case TAB:
    {
      /* Print tab key (4 spaces) and add to buffer */
      uint32_t x;
      for(x = 0; x < 4; x++){
        putc(' ');
        temp = key_index[current_terminal];
        key_buffer[current_terminal][temp] = ' ';
        key_index[current_terminal]++;

        /* Update cursor                                                                */
        update_cursor(get_screen_x(), get_screen_y());
      }
      goto SEND_EOI;
    }
    case BACK:
    {
      temp = key_index[current_terminal];
      if((key_index[current_terminal] - clear_offset[current_terminal]) != 0)
      {
        /* Print backspace */
        print_backspace();

        /* Update keyboard buffer */
        temp--;
        key_buffer[current_terminal][temp] = '\0';
      }

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
      temp = key_index[current_terminal];

      /* put newline character in buffer */
      key_buffer[current_terminal][temp] = '\n';
      putc('\n');
      wrapped = 0;
      key_index[current_terminal] = 0;
      clear_offset[current_terminal] = 0;
      current_line++;

      /* set enter flag */
      /* Should trigger terminal_write */
      enter_down = 1;

      goto SEND_EOI;
    }
    case ENTER_OFF:
    {
      enter_down = 0;

      goto SEND_EOI;
    }
    case F1: /* Handle switch to 1st terminal */
    {
        if(alt_check && (current_terminal != 0))
        {
            term_switch(0);
        }
    }
    case F2:
    {
        if(alt_check && (current_terminal != 1))
        {
            term_switch(1);
        }
    }
    case F3:
    {
        if(alt_check && (current_terminal != 2))
        {
            term_switch(2);
        }
    }
    default:
    {
      uint8_t temp;
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

        // Clear screen upon ctrl+L
        if(ctrl_check && (input == 'l' || input == 'L'))
        {
          /* Re-set screen and buffer */
          clear();
          if(key_index[current_terminal] != 0)
          {
            clear_offset[current_terminal] = key_index[current_terminal];
          }
          else
          {
            clear_buffer();
          }

          /* Update cursor */
          update_cursor(0, 0);
          puts("391OS> ");
          update_cursor(SHELL_OFFSET, 0);

          set_screen_y(0);

          current_line = 0;

          /* End interrupt */
          send_eoi(1);
          return 0;
        }

        /* Get current screen_x and screen_y */
        temp_x = get_screen_x() + 1;
        temp_y = get_screen_y() + 1;

        /* Update cursor  */
        if(temp_x == 80 && !wrapped)
        {
          /* Print letter to screen */
          putc(input);

          temp = key_index[current_terminal];
          /* Load keyboard buffer with symbol */
          key_buffer[current_terminal][temp] = input;

          /* Update index in keyboard buffer */
          key_index[current_terminal]++;

          /* Check if screen_Y has reached end of screen */
          if(temp_y == NUM_ROWS)
          {

            set_screen_y(temp_y);
            scroll_handle();
          }
          else
          {

            wrap_around();
          }

          /* Declare wrapped and update line to print to after handling */
          wrapped = 1;
          current_line = temp_y;
          update_cursor(0, temp_y);
        }
        else if(key_index[current_terminal] < MAX_BUFF_LENGTH)
        {
          /* Print letter to screen */
          putc(input);

          temp = key_index[current_terminal];

          /* Load keyboard buffer with symbol */
          key_buffer[current_terminal][temp] = input;

          /* Update index in keyboard buffer */
          key_index[current_terminal]++;

          /* Update cursor until, buffer is full */
          update_cursor(get_screen_x(), get_screen_y());
        }
      }
    }
  }

  SEND_EOI:
      /* Send interrupt signal for keyboard, the first IRQ */
      send_eoi(1);
      return 0;
}
