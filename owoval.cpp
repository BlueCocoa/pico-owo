//
//  owoval.cpp
//  OwO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OwOlang. All rights reserved.
//

#include "owoval.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <cmath>
#include "mpc.h"

const char * owotype_name(int t) {
    switch(t) {
        case OwOVAL_FUNC: return "Function";
        case OwOVAL_NUM: return "Number";
        case OwOVAL_ERR: return "Error";
        case OwOVAL_SYM: return "Symbol";
        case OwOVAL_STR: return "String";
        case OwOVAL_SEXPR: return "S-Expression";
        case OwOVAL_QEXPR: return "Q-Expression";
        default: return "Unknown";
    }
}

owoval * owoval_err(const char * fmt, ...) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_ERR;
    
    /* Create a va list and initialize it */
    va_list va;
    va_start(va, fmt);
    
    /* Allocate 512 bytes of space */
    v->err = (char *)malloc(512);
    
    /* printf the error string with a maximum of 511 characters */
    vsnprintf(v->err, 511, fmt, va);
    
    /* Reallocate to number of bytes actually used */
    v->err = (char *)realloc(v->err, strlen(v->err)+1);
    
    /* Cleanup our va list */
    va_end(va);
    
    return v;
}

owoval * owoval_num(double x) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_NUM;
    v->num = x;
    return v;
}

owoval * owoval_sym(const char* s) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_SYM;
    v->sym = (char *)malloc(strlen(s) + 1);
    strcpy(v->sym, s);
    return v;
}

owoval * owoval_str(const char * s) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_STR;
    v->str = (char *)malloc(strlen(s) + 1);
    strcpy(v->str, s);
    return v;
}

owoval * owoval_sexpr(void) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_SEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

owoval * owoval_qexpr(void) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_QEXPR;
    v->count = 0;
    v->cell = NULL;
    return v;
}

owoval * owoval_read(mpc_ast_t* t) {
    
    /* If Symbol or Number return conversion to that type */
    if (strstr(t->tag, "number")) { return owoval_read_num(t); }
    if (strstr(t->tag, "symbol")) { return owoval_sym(t->contents); }
    if (strstr(t->tag, "string")) { return owoval_read_str(t); }
    
    /* If root (>) or sexpr then create empty list */
    owoval * x = NULL;
    if (strcmp(t->tag, ">") == 0) { x = owoval_sexpr(); }
    if (strstr(t->tag, "sexpr"))  { x = owoval_sexpr(); }
    if (strstr(t->tag, "qexpr"))  { x = owoval_qexpr(); }
    
    /* Fill this list with any valid expression contained within */
    for (int i = 0; i < t->children_num; i++) {
        if (strcmp(t->children[i]->contents, "[") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "]") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
        if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
        if (strcmp(t->children[i]->tag,  "regex") == 0) { continue; }
        if (strstr(t->children[i]->tag, "comment")) { continue; }
        x = owoval_add(x, owoval_read(t->children[i]));
    }
    
    return x;
}

owoval * owoval_read_num(mpc_ast_t* t) {
    errno = 0;
    //return owoval_value(t->contents);
        double x = strtod(t->contents, NULL);
        return errno != ERANGE ?
        owoval_num(x) : owoval_err("invalid number");
}

owoval * owoval_read_str(mpc_ast_t* t) {
    /* Cut off the final quote character */
    t->contents[strlen(t->contents)-1] = '\0';
    /* Copy the string missing out the first quote character */
    char* unescaped = (char *)malloc(strlen(t->contents+1)+1);
    strcpy(unescaped, t->contents+1);
    
    /* Construct a new lval using the string */
    owoval* str = owoval_str(unescaped);
    /* Free the string and return */
    free(unescaped);
    return str;
}

owoval * owoval_lambda(owoval * formals, owoval * body) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_FUNC;
    
    /* Set Builtin to Null */
    v->builtin = NULL;
    
    /* Build new environment */
    v->env = owoenv_new();
    
    /* Set Formals and Body */
    v->formals = formals;
    v->body = body;
    return v;
}

owoval * owoval_builtin(owobuiltin func) {
    owoval * v = (owoval *)malloc(sizeof(owoval));
    v->type = OwOVAL_FUNC;
    v->builtin = func;
    return v;
}

owoval * owoval_add(owoval * v, owoval * x) {
    v->count++;
    v->cell = (struct owoval **)realloc(v->cell, sizeof(owoval *) * v->count);
    v->cell[v->count-1] = x;
    return v;
}

void owoval_print(owoval * v) {
    switch (v->type) {
            /* In the case the type is a number print it */
            /* Then 'break' out of the switch. */
        case OwOVAL_NUM: printf("%g", v->num); break;
        case OwOVAL_FUNC:
            if (v->builtin) {
                printf("<function>");
            } else {
                printf("[\\ "); owoval_print(v->formals);
                putchar(' '); owoval_print(v->body); putchar(']');
            }
            break;
            
            /* In the case the type is an error */
        case OwOVAL_ERR:
            /* Check what type of error it is and print it */
            printf("Error: %s", v->err); break;
        case OwOVAL_SYM:   printf("%s", v->sym); break;
        case OwOVAL_STR:   owoval_print_str(v); break;
        case OwOVAL_SEXPR: owoval_expr_print(v, '[', ']'); break;
        case OwOVAL_QEXPR: owoval_expr_print(v, '{', '}'); break;
    }
}

void owoval_println(owoval * v) {
    owoval_print(v);
    putchar('\n');
}

void owoval_print_str(owoval * v) {
    /* Make a Copy of the string */
    char * escaped = (char *)malloc(strlen(v->str)+1);
    strcpy(escaped, v->str);
    /* Pass it through the unescape function */
    escaped = (char *)mpcf_unescape(escaped);
    /* Print it between " characters */
    printf("%s", escaped);
    /* free the copied string */
    free(escaped);
}

void owoval_expr_print(owoval * v, char open, char close) {
    putchar(open);
    for (int i = 0; i < v->count; i++) {
        
        /* Print Value contained within */
        owoval_print(v->cell[i]);
        
        /* Don't print trailing space if last element */
        if (i != (v->count-1)) {
            putchar(' ');
        }
    }
    putchar(close);
}

owoval * owoval_eval(owoenv * e, owoval * v, bool debug) {
    if (v->type == OwOVAL_SYM) {
        owoval * x = owoenv_get(e, v);
        owoval_del(v);
        return x;
    }
    if (debug) {
        printf("v->type: %d, %d\n", v->type, v->type == OwOVAL_SEXPR);
    }
    /* Evaluate Sexpressions */
    if (v->type == OwOVAL_SEXPR) { return owoval_eval_sexpr(e, v, debug); }
    /* All other lval types remain the same */
    return v;
}

owoval * owoval_eval_sexpr(owoenv * e, owoval * v, bool debug) {
    
    /* Evaluate Children */
    for (int i = 0; i < v->count; i++) {
        if (debug) {
            printf("\n");
            owoval_print(v->cell[i]);
            printf("\n");
        }
        v->cell[i] = owoval_eval(e, v->cell[i]);
    }
    
    /* Error Checking */
    if (debug) printf("Error Checking\n");
    for (int i = 0; i < v->count; i++) {
        if (v->cell[i]->type == OwOVAL_ERR) { return owoval_take(v, i); }
    }
    if (debug) printf("end Error Checking\n");
    
    /* Empty Expression */
    if (v->count == 0) { 
        if (debug) printf("Empty Expression\n");
        return v;
    }
    
    /* Single Expression */
    if (v->count == 1) {
        if (debug) printf("Single Expression\n");
        return owoval_take(v, 0);
    }
    
    /* Ensure First Element is Symbol */
    owoval * f = owoval_pop(v, 0);
    if (f->type != OwOVAL_FUNC) {
        owoval * err = owoval_err(
                                  "S-Expression starts with incorrect type. "
                                  "Got %s, Expected %s.",
                                  owotype_name(f->type), owotype_name(OwOVAL_FUNC));
        owoval_del(f); owoval_del(v);
        return err;
    }
    
    /* Call builtin with operator */
    if (debug) printf("Call op\n");
    owoval * result = owoval_call(e, f, v);
    if (debug) printf("end Call op\n");
    owoval_del(f);
    if (debug) printf("end eval\n");
    return result;
}

owoval * owoval_copy(owoval* v) {
    
    owoval * x = (owoval *)malloc(sizeof(owoval));
    x->type = v->type;
    
    switch (v->type) {
            
            /* Copy Functions and Numbers Directly */
        case OwOVAL_FUNC:
            if (v->builtin) {
                x->builtin = v->builtin;
            } else {
                x->builtin = NULL;
                x->env = owoenv_copy(v->env);
                x->formals = owoval_copy(v->formals);
                x->body = owoval_copy(v->body);
            }
            break;
        case OwOVAL_NUM: x->num = v->num; break;
            
            /* Copy Strings using malloc and strcpy */
        case OwOVAL_ERR:
            x->err = (char *)malloc(strlen(v->err) + 1);
            strcpy(x->err, v->err); break;
            
        case OwOVAL_SYM:
            x->sym = (char *)malloc(strlen(v->sym) + 1);
            strcpy(x->sym, v->sym); break;
        case OwOVAL_STR:
            x->str = (char *)malloc(strlen(v->str) + 1);
            strcpy(x->str, v->str); break;
            
            /* Copy Lists by copying each sub-expression */
        case OwOVAL_SEXPR:
        case OwOVAL_QEXPR:
            x->count = v->count;
            x->cell = (owoval **)malloc(sizeof(owoval *) * x->count);
            for (int i = 0; i < x->count; i++) {
                x->cell[i] = owoval_copy(v->cell[i]);
            }
            break;
    }
    
    return x;
}

owoval * owoval_pop(owoval * v, int i) {
    /* Find the item at "i" */
    owoval * x = v->cell[i];
    
    /* Shift memory after the item at "i" over the top */
    memmove(&v->cell[i], &v->cell[i+1],
            sizeof(owoval *) * (v->count-i-1));
    
    /* Decrease the count of items in the list */
    v->count--;
    
    /* Reallocate the memory used */
    v->cell = (owoval **)realloc(v->cell, sizeof(owoval *) * v->count);
    return x;
}

owoval * owoval_take(owoval * v, int i) {
    owoval * x = owoval_pop(v, i);
    owoval_del(v);
    return x;
}

owoval * owoval_join(owoval * x, owoval * y) {
    
    /* For each cell in 'y' add it to 'x' */
    while (y->count) {
        x = owoval_add(x, owoval_pop(y, 0));
    }
    
    /* Delete the empty 'y' and return 'x' */
    owoval_del(y);
    return x;
}

owoval * owoval_call(owoenv* e, owoval * f, owoval * a) {
    
    /* If Builtin then simply apply that */
    if (f->builtin) { return f->builtin(e, a); }
    
    /* Record Argument Counts */
    int given = a->count;
    int total = f->formals->count;
    
    /* While arguments still remain to be processed */
    while (a->count) {
        
        /* If we've ran out of formal arguments to bind */
        if (f->formals->count == 0) {
            owoval_del(a);
            return owoval_err("Function passed too many arguments. "
                              "Got %i, Expected %i.", given, total);
        }
        
        /* Pop the first symbol from the formals */
        owoval * sym = owoval_pop(f->formals, 0);
        
        /* Special Case to deal with '&' */
        if (strcmp(sym->sym, "&") == 0) {
            
            /* Ensure '&' is followed by another symbol */
            if (f->formals->count != 1) {
                owoval_del(a);
                return owoval_err("Function format invalid. "
                                  "Symbol '&' not followed by single symbol.");
            }
            
            /* Next formal should be bound to remaining arguments */
            owoval * nsym = owoval_pop(f->formals, 0);
            owoenv_put(f->env, nsym, builtin_list(e, a));
            owoval_del(sym); owoval_del(nsym);
            break;
        }
        
        /* Pop the next argument from the list */
        owoval * val = owoval_pop(a, 0);
        
        /* Bind a copy into the function's environment */
        owoenv_put(f->env, sym, val);
        
        /* Delete symbol and value */
        owoval_del(sym); owoval_del(val);
    }
    
    /* Argument list is now bound so can be cleaned up */
    owoval_del(a);
    
    /* If '&' remains in formal list bind to empty list */
    if (f->formals->count > 0 &&
        strcmp(f->formals->cell[0]->sym, "&") == 0) {
        
        /* Check to ensure that & is not passed invalidly. */
        if (f->formals->count != 2) {
            return owoval_err("Function format invalid. "
                              "Symbol '&' not followed by single symbol.");
        }
        
        /* Pop and delete '&' symbol */
        owoval_del(owoval_pop(f->formals, 0));
        
        /* Pop next symbol and create empty list */
        owoval * sym = owoval_pop(f->formals, 0);
        owoval * val = owoval_qexpr();
        
        /* Bind to environment and delete */
        owoenv_put(f->env, sym, val);
        owoval_del(sym); owoval_del(val);
    }
    
    /* If all formals have been bound evaluate */
    if (f->formals->count == 0) {
        
        /* Set environment parent to evaluation environment */
        f->env->par = e;
        
        /* Evaluate and return */
        return builtin_eval(f->env, owoval_add(owoval_sexpr(), owoval_copy(f->body)));
    } else {
        /* Otherwise return partially evaluated function */
        return owoval_copy(f);
    }
}

void owoval_del(owoval * v) {
    if (v == nullptr) return;
    switch (v->type) {
            /* Do nothing special for number type */
        case OwOVAL_NUM: break;
        case OwOVAL_FUNC:
            if (!v->builtin) {
                owoenv_del(v->env);
                owoval_del(v->formals);
                owoval_del(v->body);
            }
            break;
            
            /* For Err or Sym free the string data */
        case OwOVAL_ERR: free(v->err); break;
        case OwOVAL_SYM: free(v->sym); break;
        case OwOVAL_STR: free(v->str); break;
            
            /* If Sexpr|Qexpr then delete all elements inside */
        case OwOVAL_QEXPR:
        case OwOVAL_SEXPR:
            for (int i = 0; i < v->count; i++) {
                owoval_del(v->cell[i]);
            }
            /* Also free the memory allocated to contain the pointers */
            free(v->cell);
            break;
    }
    
    /* Free the memory allocated for the "lval" struct itself */
    free(v);
}

int owoval_eq(owoval * x, owoval * y) {
    
    /* Different Types are always unequal */
    if (x->type != y->type) { return 0; }
    
    /* Compare Based upon type */
    switch (x->type) {
            /* Compare Number Value */
        case OwOVAL_NUM: return (x->num == y->num);
            
            /* Compare String Values */
        case OwOVAL_ERR: return (strcmp(x->err, y->err) == 0);
        case OwOVAL_SYM: return (strcmp(x->sym, y->sym) == 0);
        case OwOVAL_STR: return (strcmp(x->str, y->str) == 0);
            
            /* If builtin compare, otherwise compare formals and body */
        case OwOVAL_FUNC:
            if (x->builtin || y->builtin) {
                return x->builtin == y->builtin;
            } else {
                return owoval_eq(x->formals, y->formals)
                && owoval_eq(x->body, y->body);
            }
            
            /* If list compare every individual element */
        case OwOVAL_QEXPR:
        case OwOVAL_SEXPR:
            if (x->count != y->count) { return 0; }
            for (int i = 0; i < x->count; i++) {
                /* If any element not equal then whole list not equal */
                if (!owoval_eq(x->cell[i], y->cell[i])) { return 0; }
            }
            /* Otherwise lists must be equal */
            return 1;
            break;
    }
    return 0;
}
