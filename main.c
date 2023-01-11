#include "mpc/mpc.h"
#include "src/eval.h"

/* for Windows */
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add_history function */
void add_history(char* unused) {}

/* Otherwise include the editline headers */
#else
#include <editline/readline.h>
#include <editline/history.h>
#endif


int main(int argc, char** argv) {
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Yalang   = mpc_new("yalang");

  mpca_lang(
    MPCA_LANG_DEFAULT,
    "                                                         \
      number   : /-?[0-9]+/ ;                                 \
      operator : '+' | '-' | '*' | '/' | '%' | '^' ;          \
      expr     : <number> | '(' <operator> <expr>+ ')' ;      \
      yalang   : /^/ <operator> <expr>+ /$/ ;                 \
    ",
    Number, Operator, Expr, Yalang
  );

  puts("Yalang v0.0.1");
  puts("Press Ctrl+C to Exit\n");

  while (1) {
    char* input = readline(">>> ");
    add_history(input);

    mpc_result_t r;

    if (mpc_parse("<stdin>", input, Yalang, &r)) {
      lval result = eval(r.output);
      lval_println(result);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }
  
  mpc_cleanup(4, Number, Operator, Expr, Yalang);
  return 0;
}
