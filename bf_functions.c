#include "bf_functions.h"

void increment_pointer(size_t *pointer) {
    if (*pointer >= MEMORY_SIZE - 1) {
        printf("Attempted to access memory beyond the right bound");
        exit(1);
    }
    *pointer++;

}

void decrement_pointer(size_t *pointer) {
    if (*pointer <= 0) {
        printf("Attempted to access memory beyond the left bound");
        exit(1);
    }
    *pointer--;
}

void increment_cell(size_t pointer, unsigned char **memory) {
    if (*memory[pointer] == 0b11111111) *memory[pointer] = 0b00000000;
    else *memory[pointer]++;
}

void decrement_cell(size_t pointer, unsigned char **memory) {
    if (*memory[pointer] == 0b00000000) *memory[pointer] = 0b11111111;
    else *memory[pointer]++;
}

void input(size_t pointer, unsigned char *memory) {
    memory[pointer] = getchar();
}

void output(size_t pointer, unsigned char *memory) {
    putchar(memory[pointer]);
}
/*
void opening_jump() {

}

void closing_jump() {

}

*/