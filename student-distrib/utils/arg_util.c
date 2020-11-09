#include "arg_util.h"
#include "char_util.h"
#include "../lib.h"

/* get_argument_count
 * DESCRIPTION: Get the number of arguments in this argument string,
 *                separated by spaces
 * INPUTS: argument_string -- A string of characters with arguments
 * OUTPUTS: None
 * RETURNS: The number of arguments in this string, -1 if string is too long
 * SIDE EFFECTS: None
 */
int32_t get_argument_count(const uint8_t* argument_string)
{
    uint32_t i;
    uint32_t in_arg = 0;
    uint32_t arg_count = 0;
    int32_t length = string_length(argument_string);
    if (length < 0) return -1;
    for (i = 0; i < length; i++) {
        if ((' ' == (char)argument_string[i]) && in_arg) {
            in_arg = 0;
            arg_count++;
        } else if (' ' != (char)argument_string[i] && !in_arg) {
            in_arg = 1;
        }
    }
    if (in_arg) arg_count++;
    return arg_count;
}

/* get_argument_length
 * DESCRIPTION: Gets the length of an argument in an argument string
 * INPUTS: argument_string -- A string of characters with arguments
 *         index           -- The index within the argument string
 * OUTPUTS: None
 * RETURNS: The length of the index, -1 if fail
 * SIDE EFFECTS: None
 */
int32_t get_argument_length(const uint8_t* argument_string, uint32_t index)
{
    int32_t arg_count = get_argument_count(argument_string);
    if ((arg_count <= 0) || (index >= arg_count)) return -1;

    uint32_t i;
    uint32_t in_arg = 0;
    uint32_t arg_index = 0;
    uint32_t arg_length = 0;
    int32_t length = string_length(argument_string);
    if (length < 0) return -1;
    for (i = 0; i < length; i++) {
        if ((' ' == (char)argument_string[i]) && in_arg) {
            in_arg = 0;
            arg_index++;
        } else if (' ' != (char)argument_string[i] && !in_arg) {
            in_arg = 1;
        }
        if ((' ' != (char)argument_string[i]) && (arg_index == index)) arg_length++;
        if (arg_index > index) break;
    }
    if (arg_length == 0) return -1;  // This means there isn't any argument with this index
    return arg_length;
}

/* get_argument
 * DESCRIPTION: Gets an argument from the argument string and puts it
 *                in the buffer
 * INPUTS: argument_string -- A string of characters with arguments
 *         index           -- The index within the argument string
 *         buf             -- A buffer in which to put the argument
 * OUTPUTS: None
 * RETURNS: number of bytes copied if success, -1 if fail
 * SIDE EFFECTS: None
 */
int32_t get_argument(const uint8_t* argument_string, uint32_t index, uint8_t* buf)
{
    int32_t arg_count = get_argument_count(argument_string);
    if ((arg_count <= 0) || (index >= arg_count)) return -1;

    uint32_t i;
    uint32_t j = 0; /* index inside buffer */
    uint32_t in_arg = 0;
    uint32_t arg_index = 0;
    int32_t length = string_length(argument_string);
    if (length < 0) return -1;
    for (i = 0; i < length; i++) {
        if ((' ' == (char)argument_string[i]) && in_arg) {
            in_arg = 0;
            arg_index++;
        } else if (' ' != (char)argument_string[i] && !in_arg) {
            in_arg = 1;
        }
        if ((' ' != (char)argument_string[i])
                && (arg_index == index)
                && ('\0' != argument_string[i])) {
            buf[j] = argument_string[i];
            j++;
        }
        if (arg_index > index) break;
    }
    return j;
}

/* get_next_arguments
 * DESCRIPTION: Gets the arguments of the argument string after the first word
 * INPUTS: argument_string -- a string of arguments
 *         buf             -- a buffer in which to place the chars
 * OUTPUT: none
 * RETURNS: number of bytes copied if success, -1 if fail
 * SIDE EFFECTS: none
 */
int32_t get_next_arguments(const uint8_t* argument_string, uint8_t* buf)
{
    if (get_argument_length(argument_string, 0) <= 0) return -1; // no first

    uint32_t i = 0;
    uint32_t j = 0;
    while (argument_string[i++] == ' ');
    while (argument_string[i++] != ' ');
    while (argument_string[i++] == ' ');

    while (argument_string[i] != '\0') buf[j++] = argument_string[i++];
    buf[j] = (uint8_t)'\0';

    return j;
}


/* Returns 0 if fail, 1 if pass */
uint32_t test_arg_util() {
    uint8_t* arg1 = (uint8_t*)"test";
    uint8_t* arg2 = (uint8_t*)" test_1 test_2 ";
    uint8_t* arg3 = (uint8_t*)" foobar    barfoo   foo bar    ";

    uint8_t buf_3[3];
    uint8_t buf_4[4];
    uint8_t buf_6[6];

    uint32_t i;

    /* TEST GROUP 1 */
    if (get_argument_count(arg1) != 1) {
        printf("String '%s' should have %d arguments, not %d\n", "arg1", 1, get_argument_count(arg1));
        return 0;
    }
    if (get_argument_length(arg1, 0) != 4) {
        printf("Argument %d of '%s' should be size %d, not %d\n", 0, "arg1", 4, get_argument_length(arg1, 1));
        return 0;
    }
    if (get_argument(arg1, 0, buf_4) != 4) {
        printf("Copied %d characters from the %d argument of %s\n", get_argument(arg1, 0, buf_4), 0, "arg1");
    }
    if ((buf_4[0] != (int8_t)'t') || buf_4[3] != (int8_t)'t') {
        printf("Found the wrong %d argument in %s\n", 0, "arg1");
        for (i = 0; i < 4; i++) putc(buf_4[i]);
        return 0;
    }

    /* TEST GROUP 2 */
    if (get_argument_count(arg2) != 2) {
        printf("String '%s' should have %d arguments, not %d\n", "arg2", 2, get_argument_count(arg2));
        return 0;
    }
    if (get_argument_length(arg2, 0) != 6) {
        printf("Argument %d of '%s' should be size %d, not %d\n", 0, "arg2", 6, get_argument_length(arg2, 1));
        return 0;
    }
    if (get_argument_length(arg2, 1) != 6) {
        printf("Argument %d of '%s' should be size %d, not %d\n", 1, "arg2", 6, get_argument_length(arg2, 2));
        return 0;
    }
    if (get_argument(arg2, 0, buf_6) != 6) {
        printf("Copied %d characters from the %d argument of '%s'\n", get_argument(arg2, 0, buf_6), 0, "arg2");
        return 0;
    }
    if ((buf_6[1] != (int8_t)'e') || buf_6[4] != (int8_t)'_') {
        printf("Found the wrong %d argument in %s\n", 0, "arg2");
        return 0;
    }

    /* TEST GROUP 3 */
    if (get_argument_count(arg3) != 4) {
        printf("String '%s' should have %d arguments, not %d\n", "arg3", 4, get_argument_count(arg3));
        return 0;
    }
    if (get_argument_length(arg3, 0) != 6) {
        printf("Argument %d of '%s' should be size %d, not %d\n", 0, "arg3", 6, get_argument_length(arg3, 0));
        return 0;
    }
    if (get_argument_length(arg3, 1) != 6) {
        printf("Argument %d of '%s' should be size %d, not %d\n", 1, "arg3", 6, get_argument_length(arg3, 1));
        return 0;
    }
    if (get_argument(arg3, 0, buf_6) != 6) {
        printf("Copied %d characters from the %d argument of '%s'\n", get_argument(arg3, 0, buf_6), 0, "arg3");
        return 0;
    }
    if ((buf_6[2] != (int8_t)'o') || buf_6[5] != (int8_t)'r') {
        printf("Found the wrong %d argument in %s\n", 0, "arg3");
        return 0;
    }
    if (get_argument(arg3, 3, buf_3) != 3) {
        printf("Copied %d characters from the %d argument of '%s'\n", get_argument(arg3, 3, buf_3), 3, "arg3");
        return 0;
    }
    if ((buf_3[0] != (int8_t)'b') || buf_3[1] != (int8_t)'a') {
        printf("Found the wrong %d argument in %s\n", 3, "arg3");
        return 0;
    }
    if (get_argument(arg3, 4, buf_3) != -1) {
        printf("Copying characters should fail from the %d argument of '%s'\n", 4, "arg3");
        return 0;
    }
    return 1;
}
