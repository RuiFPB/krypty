#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

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
    // Create the parsers
    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Operator  = mpc_new("operator");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Krypty    = mpc_new("krypty");

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                           \
                number   : /-?[0-9]+\\.?[0-9]*/ ;                       \
                operator : '+' | '-' | '*' | '/' | '%' | \"add\" |      \
                           \"mul\" | \"div\" | \"mod\" ;                \
                expr     : <number> | '(' <expr> <operator> <expr> ')' ;\
                krypty   : /^/ <expr> <operator> <expr> /$/ ;           \
            ", 
            Number, Operator, Expr, Krypty);

    puts("krypty Version 0.0.2");
    puts("Developed by RuiFPB");
    puts("Check https://www.buildyourownlisp.com to build your own language");
    puts("Press Ctrl+C to Exit\n");

    while(1) {
        // Output the prompt and get input
        char *input = readline("krypty> ");

        // Add input to history
        add_history(input);

        // (Attempt to) Parse the user input
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Krypty, &r)) {
            // On success print the AST
            mpc_ast_print(r.output);
            mpc_ast_delete(r.output);
        } else {
            // Print error
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }

        // Free the input
        free(input);
    }

    // Cleanup all mpc realated stuff
    mpc_cleanup(4, Number, Operator, Expr, Krypty);
    return 0;
}
