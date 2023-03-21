#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include "main.h"

struct termios old_attributes, new_attributes;
Instruction *commands = NULL; 
unsigned char *memory = NULL;

int main(int argument_count, char *arguments[]) {
    change_terminal_behaviour();
    signal(SIGINT, signal_handler);

    if (argument_count == 1) raise_error("Expected a source file name");
    if (argument_count > 2) raise_error("Expected only one argument");

    size_t pointer = 0, command_counter = read_source_file(arguments[1]);
    memory = (unsigned char*) calloc(MEMORY_SIZE, 1);
    if (memory == NULL) raise_error("Couldn't allocate memory\n");

    for (int i = 0; i < command_counter; i++) {
        if (commands[i] == MOVE_LEFT && pointer == 0) raise_error("Attempted to move pointer beyond the left bound");
        else if (commands[i] == MOVE_LEFT) pointer--;
        else if (commands[i] == MOVE_RIGHT && pointer == MEMORY_SIZE - 1) raise_error("Attempted to move pointer beyond the right bound");
        else if (commands[i] == MOVE_RIGHT) pointer++;
        else if (commands[i] == INCREMENT && memory[pointer] == 255) memory[pointer] = 0;
        else if (commands[i] == INCREMENT) memory[pointer]++;
        else if (commands[i] == DECREMENT && memory[pointer] == 0) memory[pointer] = 255;
        else if (commands[i] == DECREMENT) memory[pointer]--;
        else if (commands[i] == INPUT) memory[pointer] = getchar();
        else if (commands[i] == OUTPUT) putchar(memory[pointer]);
        else if (commands[i] == OPEN_LOOP && memory[pointer] == 0) {
            i++;
            for (size_t bracket_counter = 0; commands[i] != CLOSE_LOOP || bracket_counter != 0; i++) {
                if (commands[i] == OPEN_LOOP) bracket_counter++;
                else if (commands[i] == CLOSE_LOOP) bracket_counter--;
            }
        } else if (commands[i] == CLOSE_LOOP && memory[pointer] != 0) { 
            i--;
            for (size_t bracket_counter = 0; commands[i] != OPEN_LOOP || bracket_counter != 0 ; i--) {
                if (commands[i] == CLOSE_LOOP) bracket_counter++;
                else if (commands[i] == OPEN_LOOP) bracket_counter--;
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes); // restore previous terminal settings
    free(commands);
    free(memory);
    return 0;
}

/* TODO: a better way to read the source file */
size_t read_source_file(char name[]) {
    FILE *file = fopen(name, "rb");
    if (file == NULL) raise_error("Couldn't open the source file");

    size_t command_counter, bracket_counter = 0;
    unsigned char buffer[BUFFER_SIZE], character = fgetc(file);
    while(!feof(file)) {
        if (character == '<') buffer[command_counter++] = MOVE_LEFT;
        else if (character == '>') buffer[command_counter++] = MOVE_RIGHT;
        else if (character == '+') buffer[command_counter++] = INCREMENT;
        else if (character == '-') buffer[command_counter++] = DECREMENT;
        else if (character == ',') buffer[command_counter++] = INPUT;
        else if (character == '.') buffer[command_counter++] = OUTPUT;
        else if (character == '[') {
            buffer[command_counter++] = OPEN_LOOP;
            bracket_counter++;
        } else if (character == ']') {
            buffer[command_counter++] = CLOSE_LOOP;
            if (bracket_counter == 0) {
                fclose(file);
                raise_error("Not all closed square brackets have a matching open bracket");
            }
            bracket_counter--;
        }
        character = fgetc(file);
    }
    fclose(file);

    if (bracket_counter != 0) raise_error("Not all open square brackets have a matching closed bracket");

    commands = (Instruction *) calloc(command_counter, sizeof (Instruction));
    if (commands == NULL) raise_error("Couldn't allocate memory\n");
    for (int i = 0; i < command_counter; i++)
        commands[i] = buffer[i];

    return command_counter;
}

void change_terminal_behaviour() {
    tcgetattr(STDIN_FILENO, &old_attributes); // get current terminal settings
    new_attributes = old_attributes;
    new_attributes.c_lflag &= ~(ICANON); // turn on canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes); // set new terminal settings
}

void raise_error(char error_message[]) {
    printf("\nError: %s\n", error_message);
    tcsetattr(STDIN_FILENO, TCSANOW, &old_attributes); // restore previous terminal settings
    free(commands);
    free(memory);
    _exit(1);
}

void signal_handler(int signal_number) {
    raise_error("Program interrupted");
}