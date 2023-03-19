#include "bf_functions.c"

int main(void) {
    unsigned char memory[MEMORY_SIZE];
    size_t pointer = 0;
    for (size_t i = 0; i < MEMORY_SIZE; i++)
        memory[i] = 0;

    FILE *file = fopen("program.bf", "rb");
    if (file == NULL) {
        printf("Error while opening the brainfuck source file");
        return 1;
    }

    char character = fgetc(file);
    while (!feof(file)) {
        printf("%c", character);
        character = fgetc(file);
    }

    fclose(file);
    return 0;
}