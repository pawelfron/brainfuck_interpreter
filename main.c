#include <stdio.h>
#include <stdlib.h>

/* Libraries for the bs below */
#include <termios.h>
#include <unistd.h>
/* ---- */

#define MOVE_LEFT 0 // <
#define MOVE_RIGHT 1 // >
#define INCREMENT 2 // +
#define DECREMENT 3 // -
#define INPUT 4 // ,
#define OUTPUT 5 // .
#define OPEN_LOOP 6 // [
#define CLOSE_LOOP 7 // ]

#define MEMORY_SIZE 1024 // size of the brainfuck memory array
#define BUFFER_SIZE 10000

char *read_source_file(char *name, size_t *command_count) {
    unsigned char buffer[BUFFER_SIZE];
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
        if (commands[i] == MOVE_LEFT) {
            if (pointer == 0) {
                printf("Attempted to move pointer beyond the left bound\n");
                return 1;
            }
            pointer--;

        } else if (commands[i] == MOVE_RIGHT) {
            if (pointer == MEMORY_SIZE - 1) {
                printf("Attempted to move pointer beyond the right bound\n");
                return 1;
            }
            pointer++;

        } else if (commands[i] == INCREMENT){
            if (memory[pointer] == 255) memory[pointer] = 0;
            else memory[pointer]++;

        } else if (commands[i] == DECREMENT) {
            if (memory[pointer] == 0) memory[pointer] = 255;
            else memory[pointer]--;

        } else if (commands[i] == INPUT) {
            memory[pointer] = getchar();

        } else if (commands[i] == OUTPUT) {
            putchar(memory[pointer]);

/* TODO
* include the case when loops are written incorrectly (possible memory leaks)
*/
        } else if (commands[i] == OPEN_LOOP && memory[pointer] == 0) {
            size_t bracket_counter = 0;
            i++;
            while (1) {
                if (commands[i] == CLOSE_LOOP && bracket_counter == 0) break;

                if (commands[i] == OPEN_LOOP) bracket_counter++;
                else if (commands[i] == CLOSE_LOOP) bracket_counter--;
                i++;
            }

        } else if (commands[i] == CLOSE_LOOP && memory[pointer] != 0) {
            size_t bracket_counter = 0;
            i--;
            while (1) {
                if (commands[i] == OPEN_LOOP && bracket_counter == 0) break;

                if (commands[i] == CLOSE_LOOP) bracket_counter++;
                else if (commands[i] == OPEN_LOOP) bracket_counter--;
                i--;
            }
        }
    }

    free(memory);
    free(commands);

    /* Reversing the changes to the terminal made by the bs above */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    /* --- */

    return 0;
}