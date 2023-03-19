#include <stdio.h>
#include <stdlib.h>

/* Libraries for the bs below */
#include <termios.h>
#include <unistd.h>
/* ---- */

void move_left(size_t *pointer); // <
void move_right(size_t *pointer); // >
void increment(unsigned char *cell); // +
void decrement(unsigned char *cell); // -
void input(unsigned char *cell); // ,
void output(unsigned char *cell); // .
void open_loop(); // [
void close_loop(); // ]

#define MOVE_LEFT 0
#define MOVE_RIGHT 1
#define INCREMENT 2
#define DECREMENT 3
#define INPUT 4
#define OUTPUT 5
#define OPEN_LOOP 6
#define CLOSE_LOOP 7

// size of the brainfuck memory array
#define MEMORY_SIZE 1024

char *read_source_file(char *name, size_t *command_count) {
    unsigned char buffer[10000];
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

    size_t pointer = 0;
    unsigned char *memory = (unsigned char*) calloc(MEMORY_SIZE, 1);

    size_t command_counter = 0;
    char *commands = read_source_file("program.bf", &command_counter);

    for (int i = 0; i < command_counter; i++) {
        if (commands[i] == MOVE_LEFT) move_left(&pointer);
        else if (commands[i] == MOVE_RIGHT) move_right(&pointer);
        else if (commands[i] == INCREMENT) increment(memory + pointer);
        else if (commands[i] == DECREMENT) decrement(memory + pointer);
        else if (commands[i] == INPUT) input(memory + pointer);
        else if (commands[i] == OUTPUT) output(memory + pointer);
        else if (commands[i] == OPEN_LOOP) open_loop();
        else if (commands[i] == CLOSE_LOOP) close_loop();
    }

    free(memory);
    free(commands);

    /* Reversing the changes to the terminal made by the bs above */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    /* --- */

    return 0;
}

void move_left(size_t *pointer) {
    if (*pointer == 0) {
        printf("Attempted to access memory beyond the left bound\n");
        exit(1);
    }
    *pointer -= 1;
}

void move_right(size_t *pointer) {
    if (*pointer == MEMORY_SIZE - 1) {
        printf("Attempted to access memory beyond the right bound\n");
        exit(1);
    }
    *pointer += 1;
}

void increment(unsigned char *cell) {
    if (*cell == 255) *cell = 0;
    else *cell += 1;
}

void decrement(unsigned char *cell) {
    if (*cell == 0) *cell = 255;
    else *cell -= 1;
}

void input(unsigned char *cell) { *cell = getchar(); }

void output(unsigned char *cell) { putchar(*cell); }

void open_loop() {
    printf("'[' not implemented\n");
}

void close_loop() {
    printf("']' not implemented\n");
}