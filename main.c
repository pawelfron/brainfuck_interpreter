#include <stdio.h>
#include <stdlib.h>

/* Libraries for the bs below */
#include <termios.h>
#include <unistd.h>
/* ---- */

void increment_pointer(size_t *pointer); // >
void decrement_pointer(size_t *pointer); // <
void increment_cell(unsigned char *cell); // +
void decrement_cell(unsigned char *cell); // -
void input(unsigned char *cell); // ,
void output(unsigned char *cell); // .
void opening_jump(size_t pointer, unsigned char *memory, FILE *file); // [
void closing_jump(size_t pointer, unsigned char *memory, FILE *file); // ]

// size of the brainfuck memory array
#define MEMORY_SIZE 1024

#define MOVE_LEFT 0
#define MOVE_RIGHT 1
#define INCREMENT 2
#define DECREMENT 3
#define INPUT 4
#define OUTPUT 5
#define OPEN_LOOP 6
#define CLOSE_LOOP 7

char *read_source_file(char *name, size_t *command_count) {
    char buffer[10000];
    size_t counter = 0;
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error while opening the brainfuck source file");
        exit(1);
    }

    char character = fgetc(file);
    while(!feof(file)) {
        if (character == '>') buffer[counter] = MOVE_LEFT;
        else if (character == '<') buffer[counter] = MOVE_RIGHT;
        else if (character == '+') buffer[counter] = INCREMENT;
        else if (character == '-') buffer[counter] = DECREMENT;
        else if (character == ',') buffer[counter] = INPUT;
        else if (character == '.') buffer[counter] = OUTPUT;
        else if (character == '[') buffer[counter] = OPEN_LOOP;
        else if (character == ']') buffer[counter] = CLOSE_LOOP;
        counter++;
        character = fgetc(file);
    }

    fclose(file);

    char *commands = (char *) malloc(counter);
    for (int i = 0; i < counter; i++)
        commands[i] = buffer[i];

    *command_count = counter;
    return commands;
}

int main(void) {
    /* 
    Some bs to change the behaviour of the terminal.
    It causes the getchar() function to get just a single character from stdin, without the need to press enter.
    I have no clue how any of this works, but it works.

    based on ChatGPT answer and someone answering this question on Stack Overflow: https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only
    */
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    /* --- */

    unsigned char *memory = (unsigned char*) calloc(MEMORY_SIZE, 1);
    size_t pointer = 0;

    size_t command_count = 0;
    char *commands = read_source_file("program.bf", &command_count);

    for (int i = 0; i < command_count; i++) {
        if (commands[i] == MOVE_LEFT) increment_pointer(&pointer);
        else if (commands[i] == MOVE_RIGHT) decrement_pointer(&pointer);
        else if (commands[i] == INCREMENT) increment_cell(memory + pointer);
        else if (commands[i] == DECREMENT) decrement_cell(memory + pointer);
        else if (commands[i] == INPUT) input(memory + pointer);
        else if (commands[i] == OUTPUT) output(memory + pointer);
    }

    free(memory);
    free(commands);

    /* Reversing the changes to the terminal made by the bs above */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    /* --- */

    return 0;
}

void increment_pointer(size_t *pointer) {
    if (*pointer >= MEMORY_SIZE - 1) {
        printf("Attempted to access memory beyond the right bound\n");
        exit(1);
    }
    *pointer += 1;
}

void decrement_pointer(size_t *pointer) {
    if (*pointer <= 0) {
        printf("Attempted to access memory beyond the left bound\n");
        exit(1);
    }
    *pointer -= 1;
}

void increment_cell(unsigned char *cell) {
    if (*cell == 255) *cell = 0;
    else *cell += 1;
}

void decrement_cell(unsigned char *cell) {
    if (*cell == 0) *cell = 255;
    else *cell -= 1;
}

void input(unsigned char *cell) { *cell = getchar(); }

void output(unsigned char *cell) { putchar(*cell); }