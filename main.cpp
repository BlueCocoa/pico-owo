#include <stdio.h>
#include "pico/stdlib.h"
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include "mpc.h"
#include "owolang.h"

#define __OWOPROMPT__ "OwO>"

mpc_parser_t * number;
mpc_parser_t * symbol;
mpc_parser_t * strings;
mpc_parser_t * comment;
mpc_parser_t * sexpr;
mpc_parser_t * qexpr;
mpc_parser_t * expression;
mpc_parser_t * owo;

owoval * builtin_exit(owoenv *e, owoval * a) {
    printf("（/>///<）/~~~~~~~~~~~~~~~~~~╧═╧ )>口<) \n");
    exit(0);
    return NULL;
}

void owoenv_add_builtins(owoenv * e) {
    owoenv_add_builtin(e, "exit", builtin_exit);
    
    /* String Functions */
    owoenv_add_builtin(e, "load",  builtin_load);
    owoenv_add_builtin(e, "error", builtin_error);
    owoenv_add_builtin(e, "print", builtin_print);
    owoenv_add_builtin(e, "println", builtin_println);
    owoenv_add_builtin(e, "con", builtin_con);
    
    /* Variable Functions */
    owoenv_add_builtin(e, "\\", builtin_lambda);
    owoenv_add_builtin(e, "def", builtin_def);
    owoenv_add_builtin(e, "=",   builtin_put);
    
    owoenv_add_builtin(e, "if", builtin_if);
    owoenv_add_builtin(e, "==", builtin_eq);
    owoenv_add_builtin(e, "!=", builtin_ne);
    owoenv_add_builtin(e, ">",  builtin_gt);
    owoenv_add_builtin(e, "<",  builtin_lt);
    owoenv_add_builtin(e, ">=", builtin_ge);
    owoenv_add_builtin(e, "<=", builtin_le);
    
    /* List Functions */
    owoenv_add_builtin(e, "list", builtin_list);
    owoenv_add_builtin(e, "head", builtin_head);
    owoenv_add_builtin(e, "tail", builtin_tail);
    owoenv_add_builtin(e, "eval", builtin_eval);
    owoenv_add_builtin(e, "join", builtin_join);
    
    /* control flow */
    owoenv_add_builtin(e, "loop", builtin_loop);
    
    /* Mathematical Functions */
    owoenv_add_builtin(e, "+", builtin_add);
    owoenv_add_builtin(e, "-", builtin_sub);
    owoenv_add_builtin(e, "*", builtin_mul);
    owoenv_add_builtin(e, "/", builtin_div);
    owoenv_add_builtin(e, "^", builtin_power);
    owoenv_add_builtin(e, "%", builtin_mod);
    owoenv_add_builtin(e, "in", builtin_in);
    
    /* owostring */
    owoenv_add_builtin(e, "len", len);
    owoenv_add_builtin(e, "cmp", cmp);
    owoenv_add_builtin(e, "strn", strn);
    owoenv_add_builtin(e, "endswith", endswith);
    owoenv_add_builtin(e, "hash", hash);

    /* owopico */
    owoenv_add_builtin(e, "gpio_init", builtin_gpio_init);
    owoenv_add_builtin(e, "gpio_set_dir", builtin_gpio_set_dir);
    owoenv_add_builtin(e, "gpio_put", builtin_gpio_put);
    owoenv_add_builtin(e, "sleep_ms", builtin_sleep_ms);
}

std::string getline(const char * prompt) {
    std::stringstream input;
    printf("%s ", prompt);
    while (true) {
        char c = getchar();
        if (c == 13) {
            return input.str();
        } else if (c == 127) {
            std::string s = input.str();
            if (!s.empty()) {
                s[s.length() - 1] = ' ';
                printf("\r%s %s", prompt, s.c_str());
                s.erase(std::prev(s.end()));
                printf("\r%s %s", prompt, s.c_str());
            }
            input.clear();
            input.str("");
            input << s;
            continue;
        } else {
            input << c;
        }
        printf("\r%s %s", prompt, input.str().c_str());
    }
}

int main() {
    stdio_init_all();
    number = mpc_new("number");
    symbol = mpc_new("symbol");
    strings = mpc_new("string");
    comment = mpc_new("comment");
    sexpr = mpc_new("sexpr");
    qexpr = mpc_new("qexpr");
    expression = mpc_new("expression");
    owo = mpc_new("owo");
    
    mpca_lang(MPCA_LANG_DEFAULT,
              "                                                            \
              number     : /-?[0-9]+\\.?[0-9]*/ ;                          \
              symbol     : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;              \
              string     : /\"(\\\\.|[^\"])*\"/ ;                          \
              comment    : /;[^\\r\\n]*/ ;                                 \
              sexpr      : '[' <expression>* ']' ;                         \
              qexpr      : '{' <expression>* '}' ;                         \
              expression : <number> | <symbol> | <string>                  \
              | <comment> | <sexpr> | <qexpr> ;                 \
              owo        : /^/ <expression>* /$/ ;                         \
              ",
              number, symbol, strings, comment, sexpr, qexpr, expression, owo);
    owoenv * e = owoenv_new();
    owoenv_add_builtins(e);

    printf("OwO Programming Language v0.0.1\nBuilt at %s compiled with gcc version %s\n", __DATE__, __VERSION__);
    while (1) {
        std::string input = getline(__OWOPROMPT__);
        mpc_result_t r;
        if (mpc_parse("<stdin>", input.c_str(), owo, &r)) {
            /* On Success Print the AST */
            putchar('\n');
            owoval * x = owoval_eval(e, owoval_read((mpc_ast_t *)r.output));
            owoval_println(x);
            owoval_del(x);
            mpc_ast_delete((mpc_ast_t *)r.output);
        } else {
            /* Otherwise Print the Error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
    }
    owoenv_del(e);
    mpc_cleanup(8, number, symbol, strings, comment, sexpr, qexpr, expression, owo);
}
