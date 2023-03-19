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

#define MEMORY_SIZE 1024

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

    FILE *file = fopen("program.bf", "rb");
    if (file == NULL) {
        printf("Error while opening the brainfuck source file");
        return 1;
    }

    char character = fgetc(file);
    while (!feof(file)) {
        switch (character) {
            case '>':
                increment_pointer(&pointer);
                break;
            case '<':
                decrement_pointer(&pointer);
                break;
            case '+':
                increment_cell(memory + pointer);
                break;
            case '-':
                decrement_cell(memory + pointer);
                break;
            case ',':
                input(memory + pointer);
                break;
            case '.':
                output(memory + pointer);
                break;
        }
        character = fgetc(file);
    }

    fclose(file);

    free(memory);

    /* Reversing the changes to the terminal made by the bs above */
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr);
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
    *cell += 1;
}

void decrement_cell(unsigned char *cell) {
    if (*cell == 0) *cell = 255;
    *cell -= 1;
}

void input(unsigned char *cell) {
    *cell = getchar();
}

void output(unsigned char *cell) {
    putchar(*cell);
}