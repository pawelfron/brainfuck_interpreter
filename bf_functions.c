#include "bf_functions.h"

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
/*
void opening_jump(size_t pointer, unsigned char *memory, FILE *file) {

}

void closing_jump(size_t pointer, unsigned char *memory, FILE *file) {

}
*/