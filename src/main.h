#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h> /* library specific to UNIX-like systems */

#define MOVE_LEFT 0 // <
#define MOVE_RIGHT 1 // >
#define INCREMENT 2 // +
#define DECREMENT 3 // -
#define INPUT 4 // ,
#define OUTPUT 5 // .
#define OPEN_LOOP 6 // [
#define CLOSE_LOOP 7 // ]

#define MEMORY_SIZE 1024 /* size of the brainfuck memory array */
#define BUFFER_SIZE 5120 /* size of the buffer for loading the source file */

/* loads the entire program to memory, without comments, returns the number of instructions */
size_t read_source_file(char name[]);
/*  Changes the behaviour of the terminal: causes the getchar() function to get just a single character from stdin, without the need to press enter. Based on ChatGPT's answer and this answer on Stack Overflow: https://stackoverflow.com/a/1798833 */
void change_terminal_behaviour();
/* frees heap-allocated memory, restores previous terminal settings and exits */
void raise_error(char error_message[]);
/* custom signal handler; necessery due to changes to the default terminal behaviour */
void signal_handler(int signal_number);