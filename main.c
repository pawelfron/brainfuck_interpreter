#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* Libraries for the bs below */
#include <termios.h>
#include <unistd.h>
/* ---- */

/*
TODO validate the brackets 
TODO shorter code inside the brackets handler
*/

#define MOVE_LEFT 0 // <
#define MOVE_RIGHT 1 // >
#define INCREMENT 2 // +
#define DECREMENT 3 // -
#define INPUT 4 // ,
#define OUTPUT 5 // .
#define OPEN_LOOP 6 // [
#define CLOSE_LOOP 7 // ]

#define MEMORY_SIZE 1024 // size of the brainfuck memory array
#define BUFFER_SIZE 10000

/* global variables, so that is possible to free the pointers and to reverse the change to terminal inside the signal handler funtion */
struct termios oldattr, newattr;
unsigned char *commands = NULL, *memory = NULL; 

unsigned char *read_source_file(char *name, size_t *command_count); /* loading the entier program to memory, without comments */
void change_terminal_behaviour(); /* changing the behaviour of the terminal */
void clean(); /* freeing heap-allocated memory and reversing the change made by the above function */
void signal_handler(int signal_number); /* custom signal handler */

int main(void) {
    size_t command_counter = 0;
    commands = read_source_file("program.bf", &command_counter);

    size_t pointer = 0;
    memory = (unsigned char*) calloc(MEMORY_SIZE, 1);

    change_terminal_behaviour();

    /* setting the signal handler; here, not earlier, because the clean function frees memory from *commands and *memory, so they have to be allocated beforehand */
    signal(SIGINT, signal_handler);

    for (int i = 0; i < command_counter; i++) {
        if (commands[i] == MOVE_LEFT) {
            if (pointer == 0) {
                printf("\nAttempted to move pointer beyond the left bound\n");
                clean();
                return 1;
            }
            pointer--;

        } else if (commands[i] == MOVE_RIGHT) {
            if (pointer == MEMORY_SIZE - 1) {
                printf("\nAttempted to move pointer beyond the right bound\n");
                clean();
                return 1;
            }
            pointer++;

        } else if (commands[i] == INCREMENT){
            if (memory[pointer] == 255) memory[pointer] = 0;
            else memory[pointer]++;

        } else if (commands[i] == DECREMENT) {
            if (memory[pointer] == 0) memory[pointer] = 255;
            else memory[pointer]--;

        } else if (commands[i] == INPUT) {
            memory[pointer] = getchar();

        } else if (commands[i] == OUTPUT) {
            putchar(memory[pointer]);

        } else if (commands[i] == OPEN_LOOP && memory[pointer] == 0) {
            size_t bracket_counter = 0;
            i++;
            while (1) {
                if (commands[i] == CLOSE_LOOP && bracket_counter == 0) break;

                if (commands[i] == OPEN_LOOP) bracket_counter++;
                else if (commands[i] == CLOSE_LOOP) bracket_counter--;
                i++;
            }

        } else if (commands[i] == CLOSE_LOOP && memory[pointer] != 0) {
            size_t bracket_counter = 0;
            i--;
            while (1) {
                if (commands[i] == OPEN_LOOP && bracket_counter == 0) break;

                if (commands[i] == CLOSE_LOOP) bracket_counter++;
                else if (commands[i] == OPEN_LOOP) bracket_counter--;
                i--;
            }
        }
    }

    clean();
    return 0;
}

unsigned char *read_source_file(char *name, size_t *command_count) {
    unsigned char buffer[BUFFER_SIZE];
    size_t counter = 0;

    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error while opening the brainfuck source file");
        exit(1);
    }

    unsigned char character = fgetc(file);
    while(!feof(file)) {
        if (character == '<') buffer[counter++] = MOVE_LEFT;
        else if (character == '>') buffer[counter++] = MOVE_RIGHT;
        else if (character == '+') buffer[counter++] = INCREMENT;
        else if (character == '-') buffer[counter++] = DECREMENT;
        else if (character == ',') buffer[counter++] = INPUT;
        else if (character == '.') buffer[counter++] = OUTPUT;
        else if (character == '[') buffer[counter++] = OPEN_LOOP;
        else if (character == ']') buffer[counter++] = CLOSE_LOOP;

        character = fgetc(file);
    }

    fclose(file);

    unsigned char *commands = (unsigned char *) malloc(counter);
    for (int i = 0; i < counter; i++)
        commands[i] = buffer[i];

    *command_count = counter;
    return commands;
}

void change_terminal_behaviour() {
    /* 
    Some bs to change the behaviour of the terminal.
    It causes the getchar() function to get just a single character from stdin, without the need to press enter.
    I have no clue how any of this works, but it works.

    based on ChatGPT answer and someone answering this question on Stack Overflow: https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only
    */
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
}

void clean() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    free(commands);
    free(memory);
}

void signal_handler(int signal_number) {
    printf("\nProgram interrupted\n");
    clean();
    exit(1);
}