#include <stdio.h>
#include <stdlib.h>

// If we are on crappy OS
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

// fake readline
char *readline(char *prompt) {
    fputs(prompt, stdout);
    fgets(buffer, 2048, stdin);
    char *cpy = malloc(strlen(buffer) + 1);
    strcpy(cpy, buffer);
    cpy[strlen(cpy) - 1] = '\0';
    return cpy;
}

// fake add_history
void add_history(char *unused) {}


#else
#include <editline/readline.h>
#include <editline/history.h>
#endif


int main(int argc, char **argv) {
    puts("krypty Version 0.0.1");
    puts("Developed by RuiFPB");
    puts("Check https://www.buildyourownlisp.com to build your own language");
    puts("Press Ctrl+C to Exit\n");

    while(1) {
        // Output the prompt and get input
        char *input = readline("krypty> ");

        // Add input to history
        add_history(input);

        // Echo it back to the user
        printf("Input from user: %s\n", input);

        // Free the input
        free(input);
    }

    return 0;
}
