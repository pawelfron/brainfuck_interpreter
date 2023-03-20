#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h> /* Library specific to UNIX-like systems */

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

/* global variables, so that is possible to free heap-allocated memory and to restore previous terminal settigns from the signal_handler funtion (which can't take custom arguments) */
struct termios oldattr, newattr;
unsigned char *commands = NULL, *memory = NULL; 

size_t read_source_file(char *name); /* loads the entire program to memory, without comments, returns the number of instructions */
void change_terminal_behaviour(); /* changes the behaviour of the terminal */
void raise_error(char *error_message); /* frees heap-allocated memory, restores the previous terminal settings and exits */
void signal_handler(int signal_number); /* custom signal handler; necessery due to changes to the default terminal behaviour */

int main(void) {
    change_terminal_behaviour();
    signal(SIGINT, signal_handler);

    size_t pointer = 0, command_counter = read_source_file("program.bf");
    memory = (unsigned char*) calloc(MEMORY_SIZE, 1);
    if (memory == NULL) raise_error("Couldn't allocate memory\n");

    for (int i = 0; i < command_counter; i++) {
        if (commands[i] == MOVE_LEFT && pointer == 0) raise_error("Attempted to move pointer beyond the left bound");
        else if (commands[i] == MOVE_LEFT) pointer--;
        else if (commands[i] == MOVE_RIGHT && pointer == MEMORY_SIZE - 1) raise_error("Attempted to move pointer beyond the right bound");
        else if (commands[i] == MOVE_RIGHT) pointer++;
        else if (commands[i] == INCREMENT && memory[pointer] == 255) memory[pointer] = 0;
        else if (commands[i] == INCREMENT) memory[pointer]++;
        else if (commands[i] == DECREMENT && memory[pointer] == 0) memory[pointer] = 255;
        else if (commands[i] == DECREMENT) memory[pointer]--;
        else if (commands[i] == INPUT) memory[pointer] = getchar();
        else if (commands[i] == OUTPUT) putchar(memory[pointer]);
        else if (commands[i] == OPEN_LOOP && memory[pointer] == 0) {
            i++;
            for (size_t bracket_counter = 0; commands[i] != CLOSE_LOOP || bracket_counter != 0; i++) {
                if (commands[i] == OPEN_LOOP) bracket_counter++;
                else if (commands[i] == CLOSE_LOOP) bracket_counter--;
            }
        } else if (commands[i] == CLOSE_LOOP && memory[pointer] != 0) { 
            i--;
            for (size_t bracket_counter = 0; commands[i] != OPEN_LOOP || bracket_counter != 0 ; i--) {
                if (commands[i] == CLOSE_LOOP) bracket_counter++;
                else if (commands[i] == OPEN_LOOP) bracket_counter--;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); /* restore the previous terminal settings */
    free(commands);
    free(memory);
    return 0;
}

/* TODO read the source file in 1kb chunks */
size_t read_source_file(char *name) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) raise_error("Couldn't open the source file");

    size_t command_counter, bracket_counter = 0;
    unsigned char buffer[BUFFER_SIZE], character = fgetc(file);
    while(!feof(file)) {
        if (character == '<') buffer[command_counter++] = MOVE_LEFT;
        else if (character == '>') buffer[command_counter++] = MOVE_RIGHT;
        else if (character == '+') buffer[command_counter++] = INCREMENT;
        else if (character == '-') buffer[command_counter++] = DECREMENT;
        else if (character == ',') buffer[command_counter++] = INPUT;
        else if (character == '.') buffer[command_counter++] = OUTPUT;
        else if (character == '[') {
            buffer[command_counter++] = OPEN_LOOP;
            bracket_counter++;
        } else if (character == ']') {
            buffer[command_counter++] = CLOSE_LOOP;
            if (bracket_counter == 0) {
                fclose(file);
                raise_error("Square brackets don't match");
            }
            bracket_counter--;
        }

        character = fgetc(file);
    }

    fclose(file);

    if (bracket_counter != 0) raise_error("Square brackets don't match");

    commands = (unsigned char *) malloc(command_counter);
    if (commands == NULL) raise_error("Couldn't allocate memory\n");
    for (int i = 0; i < command_counter; i++)
        commands[i] = buffer[i];

    return command_counter;
}

void change_terminal_behaviour() {
    /*  The code below causes the getchar() function to get just a single character from stdin, without the need to press enter. Based on ChatGPT's answer and this answer on Stack Overflow: https://stackoverflow.com/a/1798833 */
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
}

void raise_error(char *error_message) {
    printf("\nError: %s\n", error_message);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); /* restore the previous terminal settings */
    free(commands);
    free(memory);
    _exit(1);
}

void signal_handler(int signal_number) {
    raise_error("Program interrupted");
}
