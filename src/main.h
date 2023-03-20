#define MOVE_LEFT 0 // <
#define MOVE_RIGHT 1 // >
#define INCREMENT 2 // +
#define DECREMENT 3 // -
#define INPUT 4 // ,
#define OUTPUT 5 // .
#define OPEN_LOOP 6 // [
#define CLOSE_LOOP 7 // ]

#define MEMORY_SIZE 1024 // Size of the brainfuck internal memory array
#define BUFFER_SIZE 5120 // Size of the buffer for reading the source file

/* Global variables, so that it is possible to access them from the signal_handler funtion (which can't take custom arguments) */
extern struct termios old_attributes; //Old terminal settings
extern struct termios new_attributes; // New terminal settings, after they are altered by change_terminal_behaviour
extern unsigned char *commands; // Pointer to the internal represantion of the interpreted program, allocated dynamically
extern unsigned char *memory; // Pointer to the brainfuck internal memory array, allocated dynamically

size_t read_source_file(char name[]); // Loads the entire program to memory, without comments, returns the number of instructions
void change_terminal_behaviour(); // Turns on canonical mode which, among other things, causes the getchar() function to get just a single character from stdin, without the need to press enter. Code based on ChatGPT's answer and this answer on Stack Overflow: https://stackoverflow.com/a/1798833
void raise_error(char error_message[]); // Frees heap-allocated memory, restores old terminal settings and exits
void signal_handler(int signal_number); // Custom signal handler; necessery to restore the default terminal behaviour after execution is interrupted by CTRL-C