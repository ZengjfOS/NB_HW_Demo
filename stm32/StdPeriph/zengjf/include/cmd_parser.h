#ifndef __CMD_PARSER_H
#define	__CMD_PARSER_H

#include "zengjf.h"

#define countof(a)   (sizeof(a) / sizeof(*(a)))

/**
 * Definition of a single element
 */
typedef struct
{
    /**
     * Name of the element
     */
    const char     *cmd;
    
    /**
     * Name of the element
     */
    const char     *desc;

    /**
     * Value of the elment
     */
    int (*func)(char *args);

} element_t;

/**
 * All elements of the list
 */
typedef struct
{
    /**
     * Number of elements
     */
    short   count;

    /**
     * The elements
     */
    element_t   cmds[];
} shell_cmds;

extern char *welcome_msg;
extern shell_cmds microcli_shell_cmds;


/* "shell_process" takes a command line as input,
    and "libshell" does the rest for you (check if
    the command is defined, call the callback function,
    provide arguments form the commandline to the callback)
*/
int shell_process(char *cmd_line);

void get_cmd_parser_char(void);

#endif /* __CMD_PARSER_H */
