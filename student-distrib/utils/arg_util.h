/* Utility functions for parsing arguments */
#include "../types.h"

/* Get number of arguments in string */
int32_t get_argument_count(const uint8_t* argument_string);
/* Get argument size */
int32_t get_argument_length(const uint8_t* argument_string, uint32_t index);
/* Get argument */
int32_t get_argument(const uint8_t* argument_string, uint32_t index, uint8_t* buf);
/* Gets the arguments after the first one */
int32_t get_next_arguments(const uint8_t* argument_string, uint8_t* buf);

uint32_t test_arg_util();
