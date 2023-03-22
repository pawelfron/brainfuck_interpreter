typedef enum {
    MOVE_LEFT = 1, // <
    MOVE_RIGHT = 2, // >
    INCREMENT = 3, // +
    DECREMENT = 4, // -
    INPUT = 5, // ,
    OUTPUT = 6, // .
    OPEN_LOOP = 7, // [
    CLOSE_LOOP = 8 // ]
} __attribute__((__packed__)) Instruction;

#define MEMORY_SIZE 32768 // Size of the brainfuck internal memory array
#define BUFFER_SIZE 1024 // Size of the buffer for reading the source file

/* Global variables, so that it is possible to access them from the signal_handler funtion (which can't take custom arguments) */
extern struct termios old_attributes; // Old terminal settings
extern struct termios new_attributes; // New terminal settings, after they are altered by change_terminal_behaviour
extern Instruction *instructions; // Pointer to the internal represantion of the interpreted program, allocated on the heap
extern unsigned char *memory; // Pointer to the brainfuck internal memory array, allocated on the heap
extern int source_file_descriptor; // Descriptor of the source file, used to close the file when there is an error

size_t read_source_file(char name[]); // Loads the program to memory in chunks, without comments, returns the number of instructions
void reallocate_instructions(size_t instruction_counter, size_t chunk_counter, unsigned char *buffer); // Reallocate the instructions array, used after one chunk of data has been read from the source file
void change_terminal_behaviour(void); // Turns on canonical mode which, among other things, causes the getchar() function to get just a single character from stdin, without the need to press enter. Code based on ChatGPT's answer and this answer on Stack Overflow: https://stackoverflow.com/a/1798833
void raise_error(char error_message[]); // Frees heap-allocated memory, restores old terminal settings, closes the source file and exits
void signal_handler(int signal_number); // Custom signal handler; necessery to restore the default terminal behaviour after execution is interrupted by CTRL-C