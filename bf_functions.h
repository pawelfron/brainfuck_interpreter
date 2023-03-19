#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024

void increment_pointer(size_t *pointer); // >
void decrement_pointer(size_t *pointer); // <
void increment_cell(unsigned char *cell); // +
void decrement_cell(unsigned char *cell); // -
void input(unsigned char *cell); // ,
void output(unsigned char *cell); // .
void opening_jump(size_t pointer, unsigned char *memory, FILE *file); // [
void closing_jump(size_t pointer, unsigned char *memory, FILE *file); // ]