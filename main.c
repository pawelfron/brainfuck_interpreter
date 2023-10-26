#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <conio.h>
    unsigned char get_character() { return getche(); }
#else
    #include <unistd.h>
    #include <termios.h>

    struct termios old_attributes, new_attributes;

    // Reverses the changes made by set_input_mode
    void reset_input_mode(void) {
        tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes);
    }

    // Turns on canonical mode which, among other things, causes the getchar() function to get just a single character from stdin, without the need to press enter. Code based on ChatGPT's answer and this answer on Stack Overflow: https://stackoverflow.com/a/1798833
    void set_input_mode(void) {
        tcgetattr(STDIN_FILENO, &old_attributes); // get current terminal settings
        new_attributes = old_attributes;
        new_attributes.c_lflag &= ~(ICANON); // turn on canonical mode
        tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes); // set new terminal settings

        atexit(reset_input_mode);
    }

    unsigned char get_character() { return getchar(); }
#endif

typedef enum {
    MOVE_LEFT, // <
    MOVE_RIGHT, // >
    INCREMENT, // +
    DECREMENT, // -
    INPUT, // ,
    OUTPUT, // .
    OPEN_LOOP, // [
    CLOSE_LOOP // ]
} Instruction;

#define MEMORY_SIZE 40000 // Size of the brainfuck internal memory array
#define BUFFER_SIZE 1000 // Size of the buffer for reading the source file

void read_source_file(char name[], Instruction **instructions, size_t *instruction_counter); // Loads the program to memory

int main(int argument_count, char *arguments[]) {
    #ifndef _WIN32
        set_input_mode();
    #endif

    if (argument_count == 1) {
        printf("Error: Expected a filename!\n");
        exit(1);
    }
    if (argument_count > 2) {
        printf("Error: Expected only one argument!\n");
        exit(1);
    }

    Instruction *instructions = NULL;
    size_t pointer = 0, instruction_counter = 0;
    read_source_file(arguments[1], &instructions, &instruction_counter);

    unsigned char *memory = (unsigned char*) calloc(MEMORY_SIZE, 1);
    if (memory == NULL) {
        printf("Error: Couldn't allocate memory!\n");
        free(instructions);
        exit(1);
    }

    for (int i = 0; i < instruction_counter; i++) {
        switch (instructions[i]) {
        case MOVE_LEFT:
            if (pointer == 0) {
                printf("Error: Attempted to move pointer beyond the left memory bound!\n");
                free(instructions);
                free(memory);
                exit(1);
            }

            pointer--;
            break;

        case MOVE_RIGHT:
            if (pointer == MEMORY_SIZE - 1) {
                printf("Error: Attempted to move pointer beyond the right memory bound!\n");
                free(instructions);
                free(memory);
                exit(1);
            }

            pointer++;
            break;

        case INCREMENT:
            if (memory[pointer] == 255) memory[pointer] = 0;
            else memory[pointer]++;
            break;

        case DECREMENT:
            if (memory[pointer] == 0) memory[pointer] = 255;
            else memory[pointer]--;
            break;

        case INPUT:
            memory[pointer] = get_character();
            break;

        case OUTPUT:
            putchar(memory[pointer]);
            break;

        case OPEN_LOOP:
            if (memory[pointer] == 0) {
                i++;
                for (size_t bracket_counter = 0; instructions[i] != CLOSE_LOOP || bracket_counter != 0; i++) {
                    if (instructions[i] == OPEN_LOOP) bracket_counter++;
                    else if (instructions[i] == CLOSE_LOOP) bracket_counter--;
                }
            }
            break;

        case CLOSE_LOOP:
            if (memory[pointer] != 0) {
                i--;
                for (size_t bracket_counter = 0; instructions[i] != OPEN_LOOP || bracket_counter != 0 ; i--) {
                    if (instructions[i] == CLOSE_LOOP) bracket_counter++;
                    else if (instructions[i] == OPEN_LOOP) bracket_counter--;
                }
            }
            break;
        }
    }

    free(instructions);
    free(memory);
    return 0;
}

void read_source_file(char name[], Instruction **instructions, size_t *instruction_counter) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) {
        printf("Error: Couldn't open the source file!\n");
        exit(1);
    }

    size_t bracket_counter = 0, chunk_counter = 0, buffer_index = 0;;
    unsigned char buffer[BUFFER_SIZE];

    for (unsigned char character = fgetc(file); !feof(file); character = fgetc(file)) {
        switch (character) {
        case '<':
            buffer[buffer_index++] = MOVE_LEFT;
            (*instruction_counter)++;
            break;

        case '>':
            buffer[buffer_index++] = MOVE_RIGHT;
            (*instruction_counter)++;
            break;

        case '+':
            buffer[buffer_index++] = INCREMENT;
            (*instruction_counter)++;
            break;

        case '-':
            buffer[buffer_index++] = DECREMENT;
            (*instruction_counter)++;
            break;

        case ',':
            buffer[buffer_index++] = INPUT;
            (*instruction_counter)++;
            break;

        case '.':
            buffer[buffer_index++] = OUTPUT;
            (*instruction_counter)++;
            break;

        case '[':
            buffer[buffer_index++] = OPEN_LOOP;
            (*instruction_counter)++;
            bracket_counter++;
            break;

        case ']':
            buffer[buffer_index++] = CLOSE_LOOP;
            (*instruction_counter)++;
            if (bracket_counter == 0) {
                printf("Error: Not all square brackets match!\n");
                free(*instructions);
                fclose(file);
                exit(1);
            }

            bracket_counter--;
            break;
        }

        // After one chunk has been read, the instructions array is reallocated, so the buffer can be used to read the next chunk
        if (buffer_index == BUFFER_SIZE - 1) {
            Instruction *new_instructions = (Instruction *) realloc(*instructions, *instruction_counter * sizeof (Instruction));
            if (new_instructions == NULL) {
                printf("Error: Couldn't allocate memory!\n");
                free(*instructions);
                fclose(file);
                exit(1);
            }
            *instructions = new_instructions;

            size_t starting_point = chunk_counter * BUFFER_SIZE;
            for (size_t i = 0; i < *instruction_counter - starting_point; i++)
                (*instructions)[starting_point + i] = buffer[i];
            
            chunk_counter++;
            buffer_index = 0;
        }
    }
    fclose(file);

    if (bracket_counter != 0) {
        printf("Error: Not all square brackets match!\n");
        free(*instructions);
        exit(1);
    }

    // Final allocation, when the last chunk of the source file didn't completely fill the buffer
    Instruction *new_instructions = (Instruction *) realloc(*instructions, *instruction_counter * sizeof (Instruction));
    if (new_instructions == NULL) {
        printf("Error: Couldn't allocate memory!\n");
        free(*instructions);
        exit(1);
    }
    *instructions = new_instructions;

    size_t starting_point = chunk_counter * BUFFER_SIZE;
    for (size_t i = 0; i < *instruction_counter - starting_point; i++)
        (*instructions)[starting_point + i] = buffer[i];
}
