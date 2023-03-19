#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 1024

void increment_pointer(size_t *pointer); // >
void decrement_pointer(size_t *pointer); // <
void increment_cell(size_t pointer, unsigned char **memory); // +
void decrement_cell(size_t pointer, unsigned char **memory); // -
void input(size_t pointer, unsigned char *memory); // .
void output(size_t pointer, unsigned char *memory); // ,
void opening_jump(); // [
void closing_jump(); // ]