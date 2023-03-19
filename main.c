/* libraries for the bs below */
#include <termios.h>
#include <unistd.h>
/* ---- */

#include "bf_functions.c"

int main(void) {

    /* 
    some bs to change the behaviour of the terminal
    it causes the getchar() function to get just single character from stdin

    based on ChatGPT answer someone answering this question on Stack Overflow
    https://stackoverflow.com/questions/1798511/how-to-avoid-pressing-enter-with-getchar-for-reading-a-single-character-only
    */
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
    /* --- */

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

    /* reversing the changes to terminal made by the bs above */
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr);
    /* --- */

    return 0;
}