#include <errno.h>
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

typedef struct {
    int type;
    union {
        int error;
        double num_dec;
        long num;
    };
} lval;

enum { LVAL_NUM, LVAL_NUM_DEC, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

// Create a new number type lval
lval lval_dec(double x) {
    lval v;
    v.type = LVAL_NUM_DEC;
    v.num_dec = x;
    return v;
}

// Create a new number type lval
lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

// Create a new error type lval
lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.error = x;
    return v;
}

// Converts numbers to decimal lval 
lval lval_num_to_dec(lval x) {
    if (x.type == LVAL_NUM) {
        x.type = LVAL_NUM_DEC;
        x.num_dec = (double)x.num;
    }
    return x;
}

// Print the lval
void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM: printf("%ld", v.num); break;
        case LVAL_NUM_DEC: printf("%lf", v.num_dec); break;
        case LVAL_ERR:
            if (v.error == LERR_DIV_ZERO) 
                printf("Error: Divide by Zero!");
            if (v.error == LERR_BAD_OP) 
                printf("Error: Invalid Operator!");
            if (v.error == LERR_BAD_NUM)
                printf("Error: Invalid Number!");
            break;
    }
}

// Print the lval and crete a new line
void lval_println(lval v) { lval_print(v); putchar('\n'); };

// Use the operator string to see the operation to perform
lval eval_op(lval x, char *op, lval y) {
#ifdef DEBUG
    printf("x: "); lval_println(x);
    printf("y: "); lval_println(y);
#endif
    if(x.type == LVAL_ERR) { return x; }
    if(y.type == LVAL_ERR) { return y; }
    if(x.type == LVAL_NUM && y.type == LVAL_NUM) {
        if(strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
        if(strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
        if(strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
        if(strcmp(op, "/") == 0) { 
            return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num / y.num);
        }
        if(strcmp(op, "%") == 0) {
            return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(x.num % y.num);
        }
    } else {
        x = lval_num_to_dec(x);
        y = lval_num_to_dec(y);
        if(strcmp(op, "+") == 0) { return lval_dec(x.num_dec + y.num_dec); }
        if(strcmp(op, "-") == 0) { return lval_dec(x.num_dec - y.num_dec); }
        if(strcmp(op, "*") == 0) { return lval_dec(x.num_dec * y.num_dec); }
        if(strcmp(op, "/") == 0) { 
            return y.num == 0 ? lval_err(LERR_DIV_ZERO) : lval_dec(x.num_dec / y.num_dec);
        }
    }
    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {

    // if is number, return
    if (strstr(t->tag, "number")) {
        errno = 0;
        long x = strtol(t-> contents, NULL, 10);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }
    if (strstr(t->tag, "decimal")) {
        errno = 0;
#ifdef DEBUG
        printf("decimal: %s\n", t->contents);
#endif
        double x = strtod(t->contents, NULL);
#ifdef DEBUG
        printf("val: %lf\n", x);
#endif
        return errno != ERANGE ? lval_dec(x) : lval_err(LERR_BAD_NUM);
    }

    // operator is always the third child
    char *op = t->children[1]->contents;

    // Store the second child in x
    lval x = eval(t->children[2]);

    // Iterate the remaining children
    if (t->children_num > 4) {
        int i = 3;
        while (strstr(t->children[i]->tag, "expr")) {
            x = eval_op(x, op, eval(t->children[i]));
            i++;
        }
    } else {
        if (strcmp(op, "-") == 0 && x.type == LVAL_NUM) x.num = -x.num;
        if (strcmp(op, "-") == 0 && x.type == LVAL_NUM_DEC) x.num_dec = -x.num_dec;
    }
    return x;
}

int main(int argc, char **argv) {
    // Create the parsers
    mpc_parser_t* Number    = mpc_new("number");
    mpc_parser_t* Decimal   = mpc_new("decimal");
    mpc_parser_t* Operator  = mpc_new("operator");
    mpc_parser_t* Expr      = mpc_new("expr");
    mpc_parser_t* Krypty    = mpc_new("krypty");

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                           \
                number   : /-?[0-9]+/ ;                                 \
                decimal  : /-?[0-9]+\\.[0-9]+/ ;                       \
                operator : '+' | '-' | '*' | '/' | '%' ;                \
                expr     : <decimal> | <number> | '(' <operator> <expr>+ ')' ;      \
                krypty   : /^/ <operator> <expr>+ /$/ ;                 \
            ", 
            Number, Decimal, Operator, Expr, Krypty);

    puts("Krypty Version 0.0.4");
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
            
#ifdef DEBUG
            mpc_ast_print(r.output);
#endif
            lval result = eval(r.output);
            lval_println(result);
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
