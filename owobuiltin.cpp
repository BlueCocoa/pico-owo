//
//  owobuiltin.cpp
//  OwO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OwOlang. All rights reserved.
//

#include "owobuiltin.h"
#include "pico/stdlib.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpc.h"

using namespace std;

extern mpc_parser_t * owo;

owoval * builtin_var(owoenv * e, owoval * a, const char * func) {
    OwOASSERT_TYPE(func, a, 0, OwOVAL_QEXPR);
    
    owoval * syms = a->cell[0];
    for (int i = 0; i < syms->count; i++) {
        OwOASSERT(a, (syms->cell[i]->type == OwOVAL_SYM),
                  "Function '%s' cannot define non-symbol. "
                  "Got %s, Expected %s.", func,
                  owotype_name(syms->cell[i]->type),
                  owotype_name(OwOVAL_SYM));
    }
    
    OwOASSERT(a, (syms->count == a->count-1),
              "Function '%s' passed too many arguments for symbols. "
              "Got %i, Expected %i.", func, syms->count, a->count-1);
    
    for (int i = 0; i < syms->count; i++) {
        /* If 'def' define in globally. If 'put' define in locally */
        if (strcmp(func, "def") == 0) {
            owoenv_def(e, syms->cell[i], a->cell[i+1]);
        }
        
        if (strcmp(func, "=")   == 0) {
            owoenv_put(e, syms->cell[i], a->cell[i+1]);
        }
    }
    
    owoval_del(a);
    return owoval_sexpr();
}

owoval * builtin_in(owoenv *e, owoval * a) {
    
    OwOASSERT(a, a->count == 2 || a->count == 3, "Function 'in' passed incorrect number of args. Expected 2 or 3.");
    OwOASSERT_TYPE("in", a, 0, OwOVAL_NUM);
    
    owoval * q = owoval_qexpr();
    
    owoval * left = owoval_pop(a, 0);
    owoval * right = owoval_pop(a, 0);
    //owoval * s = NULL;
    /*if (a->count != 0) {
     s = owoval_pop(a, 0);
     }*/
    if (left->num < right->num) {
        for (int i = left->num; i <= right->num; i++) {
            q = owoval_add(q, owoval_num((double)i));
        }
    } else {
        for (int i = left->num; i >= right->num; i--) {
            q = owoval_add(q, owoval_num((double)i));
        }
    }
    
    /*
     mpf_class from(x->value);
     mpf_class to(y->value);
     
     long step = 1;
     if (s) { step = mpf_class(s->value).get_si(); }
     
     if (from > to) {
     if (step > 0) { step = -step; }
     for (; from > to; from += step) {
     mpf_class tmp = from;
     q = owoval_add(q, owoval_value(tmp));
     }
     } else {
     if (step < 0) { step = -step; }
     for (; from < to; from += step) {
     mpf_class tmp = from;
     q = owoval_add(q, owoval_value(tmp));
     }
     }
     */
    
    owoval_del(a);
    
    return q;
}

owoval * builtin_load(owoenv * e, owoval * a) {
    OwOASSERT_NUM("load", a, 1);
    OwOASSERT_TYPE("load", a, 0, OwOVAL_STR);
    
    /* Parse File given by string name */
    mpc_result_t r;
    if (mpc_parse_contents(a->cell[0]->str, owo, &r)) {
        
        /* Read contents */
        owoval * expr = owoval_read((mpc_ast_t *)r.output);
        mpc_ast_delete((mpc_ast_t *)r.output);
        
        /* Evaluate each Expression */
        while (expr->count) {
            owoval * x = owoval_eval(e, owoval_pop(expr, 0));
            /* If Evaluation leads to error print it */
            if (x->type == OwOVAL_ERR) { owoval_println(x); }
            owoval_del(x);
        }
        
        /* Delete expressions and arguments */
        owoval_del(expr);
        owoval_del(a);
        
        /* Return empty list */
        return owoval_sexpr();
        
    } else {
        /* Get Parse Error as String */
        char* err_msg = mpc_err_string(r.error);
        mpc_err_delete(r.error);
        
        /* Create new error message using it */
        owoval * err = owoval_err("Could not load Library %s", err_msg);
        free(err_msg);
        owoval_del(a);
        
        /* Cleanup and return error */
        return err;
    }
}

owoval * builtin_error(owoenv * e, owoval * a) {
    OwOASSERT_NUM("error", a, 1);
    OwOASSERT_TYPE("error", a, 0, OwOVAL_STR);
    
    /* Construct Error from first argument */
    owoval* err = owoval_err(a->cell[0]->str);
    
    /* Delete arguments and return */
    owoval_del(a);
    return err;
}

owoval * builtin_print(owoenv * e, owoval * a) {
    
    //a->type = OwOVAL_STR;
    /* Print each argument followed by a space */
    int i;
    for (i = 0; i < a->count - 1; i++) {
        owoval_print(a->cell[i]);putchar(' ');
    }
    i++;
    owoval_print(a->cell[i]);
    
    /* Print a newline and delete arguments */
    putchar('\n');
    owoval_del(a);
    
    return owoval_sexpr();
}

owoval * builtin_println(owoenv * e, owoval * a) {
    owoval * r = builtin_print(e, a);
    putchar('\n');
    return r;
}

owoval * builtin_con(owoenv * e, owoval * a) {
    
    if (a->count == 0) {
        return owoval_err("You let me do concrete bird when you pass no argument.");
    }
    
    int i = 0;
    
    size_t size = 1;
    char * con_str_ptr = (char *)malloc(size); // automatically calculates the size of string
    con_str_ptr[0] = '\0';
    for (i = 0; i < a->count; i++) {
        size += strlen(con_str_ptr);
        con_str_ptr = (char *)realloc(con_str_ptr, size);
        if (con_str_ptr == NULL) {
            return owoval_err("Oops, out of memory");
        }
        strcpy(con_str_ptr + strlen(con_str_ptr), a->cell[i]->str);
    }
    owoval *con_str = owoval_str(con_str_ptr);
    //owoval_print(con_str);
    return con_str;
}

owoval * builtin_lambda(owoenv * e, owoval * a) {
    /* Check Two arguments, each of which are Q-Expressions */
    OwOASSERT_NUM("\\", a, 2);
    OwOASSERT_TYPE("\\", a, 0, OwOVAL_QEXPR);
    OwOASSERT_TYPE("\\", a, 1, OwOVAL_QEXPR);
    
    /* Check first Q-Expression contains only Symbols */
    for (int i = 0; i < a->cell[0]->count; i++) {
        OwOASSERT(a, (a->cell[0]->cell[i]->type == OwOVAL_SYM),
                  "Cannot define non-symbol. Got %s, Expected %s.",
                  owotype_name(a->cell[0]->cell[i]->type),owotype_name(OwOVAL_SYM));
    }
    
    /* Pop first two arguments and pass them to lval_lambda */
    owoval * formals = owoval_pop(a, 0);
    owoval * body = owoval_pop(a, 0);
    owoval_del(a);
    
    return owoval_lambda(formals, body);
}

owoval * builtin_def(owoenv * e, owoval * a) {
    return builtin_var(e, a, "def");
}
owoval * builtin_put(owoenv * e, owoval * a) {
    return builtin_var(e, a, "=");
}

owoval * builtin_if(owoenv * e, owoval* a) {
    OwOASSERT_NUM("if", a, 3);
    OwOASSERT_TYPE("if", a, 0, OwOVAL_NUM);
    OwOASSERT_TYPE("if", a, 1, OwOVAL_QEXPR);
    OwOASSERT_TYPE("if", a, 2, OwOVAL_QEXPR);
    
    /* Mark Both Expressions as evaluable */
    owoval * x;
    a->cell[1]->type = OwOVAL_SEXPR;
    a->cell[2]->type = OwOVAL_SEXPR;
    
    if (a->cell[0]->num) {
        /* If condition is true evaluate first expression */
        x = owoval_eval(e, owoval_pop(a, 1));
    } else {
        /* Otherwise evaluate second expression */
        x = owoval_eval(e, owoval_pop(a, 2));
    }
    
    /* Delete argument list and return */
    owoval_del(a);
    return x;
}

owoval * builtin_eq(owoenv * e, owoval * a) {
    return builtin_cmp(e, a, "==");
}

owoval * builtin_ne(owoenv * e, owoval * a) {
    return builtin_cmp(e, a, "!=");
}

owoval * builtin_gt(owoenv * e, owoval * a) {
    return builtin_ord(e, a, ">");
}

owoval * builtin_lt(owoenv * e, owoval * a) {
    return builtin_ord(e, a, "<");
}

owoval * builtin_ge(owoenv * e, owoval * a) {
    return builtin_ord(e, a, ">=");
}

owoval * builtin_le(owoenv * e, owoval * a) {
    return builtin_ord(e, a, "<=");
}

owoval * builtin_cmp(owoenv * e, owoval * a, const char * op) {
    OwOASSERT_NUM(op, a, 2);
    
    int r = 0;
    if (strcmp(op, "==") == 0) {
        r =  owoval_eq(a->cell[0], a->cell[1]);
    }
    if (strcmp(op, "!=") == 0) {
        r = !owoval_eq(a->cell[0], a->cell[1]);
    }
    owoval_del(a);
    return owoval_num(r);
}

owoval * builtin_ord(owoenv * e, owoval * a, const char* op) {
    OwOASSERT_NUM(op, a, 2);
    OwOASSERT_TYPE(op, a, 0, OwOVAL_NUM);
    OwOASSERT_TYPE(op, a, 1, OwOVAL_NUM);
    
    int r = 0;
    if (strcmp(op, ">")  == 0) {
        r = (a->cell[0]->num >  a->cell[1]->num);
    }
    if (strcmp(op, "<")  == 0) {
        r = (a->cell[0]->num <  a->cell[1]->num);
    }
    if (strcmp(op, ">=") == 0) {
        r = (a->cell[0]->num >= a->cell[1]->num);
    }
    if (strcmp(op, "<=") == 0) {
        r = (a->cell[0]->num <= a->cell[1]->num);
    }
    owoval_del(a);
    return owoval_num(r);
}

owoval * builtin_op(owoenv * e, owoval * a, const char* op) {
    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        int flag = 0;
        if (a->cell[i]->type == OwOVAL_QEXPR || a->cell[i]->type == OwOVAL_NUM) {
            //cout<<"Node Value: "<<a->cell[0]->cell[i]->num<<" with type: "<<a->cell[0]->cell[i]->type<<endl;
            flag = 1;
        }
        //OwOASSERT_TYPE(op, a, i, flag);
        OwOASSERT(a, flag, "Wrong data type.")
    }
    
    if (a->cell[0]->type == OwOVAL_QEXPR) {
        a = a->cell[0];
        a->type = OwOVAL_NUM;
    }
    
    /*if (a->type == OwOVAL_QEXPR) {
     owoval * tmp;
     for (int i = 0; i < a->count; i++) {
     
     owoval_add(tmp, owoval_pop(a, 0));
     }
     a = owoval_copy(tmp);
     a->type = OwOVAL_NUM;
     }*/
    
    /* Pop the first element */
    owoval * x = owoval_pop(a, 0);
    
    /* If no arguments and sub then perform unary negation */
    if ((strcmp(op, "-") == 0) && a->count == 0) {
        x->num = -x->num;
    }
    
    /* While there are still elements remaining */
    while (a->count > 0) {
        
        /* Pop the next element */
        owoval * y = owoval_pop(a, 0);
        
        
        /* Perform operation */
        if (strcmp(op, "+") == 0) { x->num += y->num; }
        if (strcmp(op, "-") == 0) { x->num -= y->num; }
        if (strcmp(op, "*") == 0) { x->num *= y->num; }
        if (strcmp(op, "^") == 0) { x->num = pow(x->num,y->num); }
        if (strcmp(op, "%") == 0) { x->num = fmod(x->num, y->num); }
        if (strcmp(op, "/") == 0) {
            if (y->num == 0) {
                owoval_del(x); owoval_del(y);
                x = owoval_err("Division By Zero.");
                break;
            }
            x->num /= y->num;
        }
        
        /* Delete element now finished with */
        owoval_del(y);
    }
    
    /* Delete input expression and return result */
    owoval_del(a);
    return x;
}

// owoval * builtin_gmp_op(owoenv * e, owoval * a, const char* op) {
//     /* Ensure all arguments are numbers */
//     for (int i = 0; i < a->count; i++) {
//         int flag = 1;
//         if (a->type == OwOVAL_QEXPR || a->type == OwOVAL_NUM) {
//             flag = 1;
//         }
//         //OwOASSERT_TYPE(op, a, i, flag);
//         OwOASSERT(a, flag, "Wrong data type.")
//     }
    
//     if (a->type == OwOVAL_QEXPR) {
//         owoval * tmp = owoval_pop(a, 0);
//         for (int i = 0; i < a->count; i++) {
//             owoval_add(tmp, owoval_pop(a, 0));
//         }
//         a = owoval_copy(tmp);
//         a->type = OwOVAL_NUM;
//     }
    
    
    
//     /* Pop the first element */
//     owoval * x = owoval_pop(a, 0);
    
//     /* If no arguments and sub then perform unary negation */
//     if ((strcmp(op, "-") == 0) && a->count == 0) {
//         char * ptr = x->value;
//         x->value = (char *)malloc(sizeof(strlen(x->value) + 2));
//         strcpy((char *)((long)(char *)x->value)+1, ptr);
//         x->value[0] = '-';
//         free(ptr);
//     }
    
//     /* While there are still elements remaining */
//     while (a->count > 0) {
        
//         /* Pop the next element */
//         owoval * y = owoval_pop(a, 0);
        
//         /* Perform operation */
        
//         mpf_class a(x->value);
//         mpf_class b(y->value);
        
//         if (strcmp(op, "+") == 0) { a += b; }
//         else if (strcmp(op, "-") == 0) { a -= b; }
//         else if (strcmp(op, "*") == 0) { a *= b; }
//         else if (strcmp(op, "/") == 0) {
//             if (b == 0) {
//                 owoval_del(x); owoval_del(y);
//                 x = owoval_err("Division By Zero.");
//                 break;
//             }
//             a /= b;
//         }
//         //        if (strcmp(op, "^") == 0) {
//         //            mpf_t res;
//         //            mpf_init2(res, '\0');
//         //            mpf_pow_ui(res, a.get_mpf_t(), b.get_mpf_t());
//         //        }
        
//         free(x->value);
//         mp_exp_t exp;
//         const char * str = a.get_str(exp, 10).c_str();
//         size_t without_dot = strlen(str);
//         if (exp == without_dot) {
//             x->value = (char *)malloc(sizeof(char) * without_dot);
//             strcpy(x->value, str);
//         } else {
//             if (exp >= 0) {
//                 x->value = (char *)malloc(sizeof(char) * without_dot + abs(exp));
//                 memcpy(x->value, str, exp);
//                 x->value[exp] = '.';
//                 memcpy((void *)((long)(char *)x->value + exp + 1), (const void *)((long)(char *)str + exp), without_dot - exp + 1);
//             } else {
//                 x->value = (char *)malloc(sizeof(char) * without_dot + abs(exp) + 1);
//                 x->value[0] = '0';
//                 x->value[1] = '.';
//                 int pos;
//                 for (pos = 2; pos < abs(exp) + 2; pos++) {
//                     x->value[pos] = '0';
//                 }
//                 memcpy((void *)((long)(char *)x->value + pos), str, without_dot);
//             }
//         }
//         /* Delete element now finished with */
//         owoval_del(y);
//     }
    
//     /* Delete input expression and return result */
//     owoval_del(a);
//     return x;
// }

owoval * builtin_list(owoenv * e, owoval * a) {
    a->type = OwOVAL_QEXPR;
    return a;
}

owoval * builtin_head(owoenv * e, owoval * a) {
    /* Check Error Conditions */
    OwOASSERT_NUM("head", a, 1);
    OwOASSERT_TYPE("head", a, 0, OwOVAL_QEXPR);
    OwOASSERT_NOT_EMPTY("head", a, 0);
    
    /* Otherwise take first argument */
    owoval * v = owoval_take(a, 0);
    
    /* Delete all elements that are not head and return */
    while (v->count > 1) { owoval_del(owoval_pop(v, 1)); }
    return v;
}

owoval * builtin_tail(owoenv * e, owoval * a) {
    /* Check Error Conditions */
    OwOASSERT(a, a->count == 1,
              "Function 'tail' passed too many arguments!");
    OwOASSERT(a, a->cell[0]->type == OwOVAL_QEXPR,
              "Function 'tail' passed incorrect type!");
    OwOASSERT(a, a->cell[0]->count != 0,
              "Function 'tail' passed {}!");
    
    /* Take first argument */
    owoval * v = owoval_take(a, 0);
    
    /* Delete first element and return */
    owoval_del(owoval_pop(v, 0));
    return v;
}

owoval * builtin_eval(owoenv * e, owoval * a) {
    OwOASSERT(a, a->count == 1,
              "Function 'eval' passed too many arguments!");
    OwOASSERT(a, a->cell[0]->type == OwOVAL_QEXPR,
              "Function 'eval' passed incorrect type!");
    
    owoval * x = owoval_take(a, 0);
    x->type = OwOVAL_SEXPR;
    return owoval_eval(e, x);
}

owoval * builtin_join(owoenv *e, owoval * a) {
    
    for (int i = 0; i < a->count; i++) {
        OwOASSERT_TYPE("join", a, i, OwOVAL_QEXPR);
    }
    
    owoval* x = owoval_pop(a, 0);
    
    while (a->count) {
        owoval * y = owoval_pop(a, 0);
        x = owoval_join(x, y);
    }
    
    owoval_del(a);
    return x;
}

owoval * builtin_add(owoenv * e, owoval * a) {
    return builtin_op(e, a, "+");
}

owoval * builtin_sub(owoenv * e, owoval * a) {
    return builtin_op(e, a, "-");
}

owoval * builtin_mul(owoenv * e, owoval * a) {
    return builtin_op(e, a, "*");
}

owoval * builtin_div(owoenv * e, owoval * a) {
    return builtin_op(e, a, "/");
}

owoval * builtin_power(owoenv * e, owoval * a) {
    return builtin_op(e, a, "^");
}

owoval * builtin_mod(owoenv * e, owoval * a) {
    return builtin_op(e, a, "%");
}

// owoval * builtin_getcwd(owoenv * e, owoval * a) {
//     char * path = getcwd(NULL, 0);
//     if (path) {
//         owoval * cwd = owoval_str(path);
//         return cwd;
//     }
//     return owoval_err("Cannot get current working directory");
// }

// owoval * builtin_import(owoenv * e, owoval * a) {
//     OwOASSERT(a, a->count >= 0,
//               "Function 'import' needs only one argument!");
    
//     owoval * cwd = builtin_getcwd(e, NULL);
//     for (int i = 0; i < a->count; i++) {
//         OwOASSERT_TYPE("import", a, i, OwOVAL_STR);
        
//         // Directly
//         owoval * args = owoval_add(owoval_sexpr(), owoval_str(a->cell[i]->str));
//         owoval * x = builtin_load(e, args);
//         if (x->type != OwOVAL_ERR) {
//             owoval_del(x);
//         }
        
//         // Relative path
//         char * relative = (char *)malloc(strlen(cwd->str) + strlen(a->cell[i]->str) + 6);
//         memset(relative, 0, strlen(cwd->str) + strlen(a->cell[i]->str) + 6);
//         strcpy(relative, cwd->str);
//         strcpy(relative + strlen(cwd->str), "/");
//         strcpy(relative + strlen(cwd->str) + 1, a->cell[i]->str);
//         strcpy(relative + strlen(cwd->str) + 1 + strlen(a->cell[i]->str), ".owo");
        
//         owoval_del(cwd);
        
//         args = owoval_add(owoval_sexpr(), owoval_str(relative));
//         free(relative);
        
//         x = builtin_load(e, args);
//         if (x->type != OwOVAL_ERR) {
//             owoval_del(x);
//         } else {
//             return owoval_err("Cannot import library '%s'", a->cell[i]->str);
//         }
//     }
//     return owoval_sexpr();
// }

owoval * builtin_loop(owoenv * e, owoval * a) {
    OwOASSERT(a, a->count > 1, "Function 'loop' expects more arguments!");
    owoval * cond = owoval_pop(a, 0);
    // int debug_count = 3;
    while (true)
    {
        // printf("enter while\n");
        // owoval_println(cond);
        owoval * copy_cond = owoval_copy(cond);
        if (copy_cond->type == OwOVAL_QEXPR) {
            copy_cond->type = OwOVAL_SEXPR;
        }
        // owoval_println(copy_cond);
        owoval * cond_eval = owoval_eval(e, copy_cond, false);
        // printf("eval cond\n");
        if (cond_eval->type != OwOVAL_NUM) {
            printf("loop 'cond' return type is not a num\n");
            owoval_print(cond_eval);
            owoval_del(cond);
            owoval_del(a);
            OwOASSERT(a, 0, "loop 'cond' return type is not a num");
        }
        // printf("eval cond: %f\n", cond_eval->num);
        if (cond_eval->num == 0) {
            owoval_del(cond_eval);
            break;
        }
        // printf("del cond: %f\n", cond_eval->num);
        owoval_del(cond_eval);

        // printf("copy body\n");
        owoval * body = a;
        // printf("\nbody: %d\n", body->count);
        // owoval_print(body);
        for (int i = 0; i < body->count; i++)
        {
            owoval * op = owoval_copy(body->cell[i]);
            // printf("\nstart owoval_print\n");
            op->type = OwOVAL_SEXPR;
            // owoval_print(op);
            // printf("\nend owoval_print\n");
            owoval_del(owoval_eval(e, op, false));
            // printf("return from eval\n");
        }
        // printf("end body\n");
        // printf("\nbody: %d\n", body->count);
        // owoval_print(body);
    }
    owoval_del(a);
    return owoval_sexpr();
}
