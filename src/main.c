#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include "main.h"

struct termios old_attributes, new_attributes;
Instruction *instructions = NULL; 
unsigned char *memory = NULL;
int source_file_descriptor;

int main(int argument_count, char *arguments[]) {
    change_terminal_behaviour();
    signal(SIGINT, signal_handler);

    if (argument_count == 1) raise_error("Expected a source file name");
    if (argument_count > 2) raise_error("Expected only one argument");

    size_t pointer = 0, instruction_counter = read_source_file(arguments[1]);
    memory = (unsigned char*) calloc(MEMORY_SIZE, 1);
    if (memory == NULL) raise_error("Couldn't allocate memory\n");

    for (int i = 0; i < instruction_counter; i++) {
        switch (instructions[i]) {
        case MOVE_LEFT:
            if (pointer == 0) raise_error("Attempted to move pointer beyond the left bound");
            pointer--;
            break;
        case MOVE_RIGHT:
            if (pointer == MEMORY_SIZE - 1) raise_error("Attempted to move pointer beyond the right bound");
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
            memory[pointer] = getchar();
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

    tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes); // restore previous terminal settings
    free(instructions);
    free(memory);
    return 0;
}

/* TODO: a better way to read the source file */
size_t read_source_file(char name[]) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) raise_error("Couldn't open the source file");
    source_file_descriptor = fileno(file);

    size_t instruction_counter = 0, bracket_counter = 0, chunk_counter = 0;
    unsigned char buffer[BUFFER_SIZE];

    for (unsigned char character = fgetc(file); !feof(file); character = fgetc(file)) {
        size_t buffer_index = instruction_counter - chunk_counter * BUFFER_SIZE;
        switch (character) {
        case '<':
            buffer[buffer_index] = MOVE_LEFT;
            instruction_counter++;
            break;
        case '>':
            buffer[buffer_index] = MOVE_RIGHT;
            instruction_counter++;
            break;
        case '+':
            buffer[buffer_index] = INCREMENT;
            instruction_counter++;
            break;
        case '-':
            buffer[buffer_index] = DECREMENT;
            instruction_counter++;
            break;
        case ',':
            buffer[buffer_index] = INPUT;
            instruction_counter++;
            break;
        case '.':
            buffer[buffer_index] = OUTPUT;
            instruction_counter++;
            break;
        case '[':
            buffer[buffer_index] = OPEN_LOOP;
            bracket_counter++;
            instruction_counter++;
            break;
        case ']':
            buffer[buffer_index] = CLOSE_LOOP;
            if (bracket_counter == 0) raise_error("Not all closed square brackets have a matching open bracket");
            bracket_counter--;
            instruction_counter++;
            break;
        }

        // After one chunk has been read, the instructions array is reallocated, so the buffer can be used to read the next chunk
        if (buffer_index == BUFFER_SIZE - 1) {
            reallocate_instructions(instruction_counter, chunk_counter, buffer);
            chunk_counter++;
        }
    }
    fclose(file);

    if (bracket_counter != 0) raise_error("Not all open square brackets have a matching closed bracket");

    // Final allocation, when the last chunk of the source file didn't completely fill the buffer
    reallocate_instructions(instruction_counter, chunk_counter, buffer);

    return instruction_counter;
}

void reallocate_instructions(size_t instruction_counter, size_t chunk_counter, unsigned char *buffer) {
    Instruction *new_instructions = (Instruction *) realloc(instructions, instruction_counter * sizeof (Instruction));
    if (new_instructions == NULL) raise_error("Couldn't allocate memory\n");
    instructions = new_instructions;

    size_t starting_point = chunk_counter * BUFFER_SIZE;
    for (size_t i = 0; i < instruction_counter - starting_point; i++)
        instructions[starting_point + i] = buffer[i];
}

void change_terminal_behaviour(void) {
    tcgetattr(STDIN_FILENO, &old_attributes); // get current terminal settings
    new_attributes = old_attributes;
    new_attributes.c_lflag &= ~(ICANON); // turn on canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes); // set new terminal settings
}

void raise_error(char error_message[]) {
    printf("\nError: %s\n", error_message);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes); // restore previous terminal settings
    close(source_file_descriptor); // in this case, it is safe to close the file even if it has already been closed - no other file will take the source file's descriptor; it is also neccesary to close it here, since the raise_error function can be called when the file is still open
    free(instructions);
    free(memory);
    _exit(1);
}

void signal_handler(int signal_number) {
    raise_error("Program interrupted");
}