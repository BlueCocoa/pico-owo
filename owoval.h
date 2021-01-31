//
//  owoval.h
//  OwO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OwOlang. All rights reserved.
//

#ifndef OWOVAL_H
#define OWOVAL_H

#include <stdio.h>
#include "owolang.h"

/**
 *  @brief  Forward Declarations
 */
struct owoval;
struct owoenv;
struct mpc_ast_t;
typedef owoval * (*owobuiltin)(owoenv *, owoval *);

typedef struct owoval {
    int type;
    
    /* Basic
     * Error and Symbol types have some string data
     */
    double num;
    char * err;
    char * sym;
    char * str;
    char * value;
    
    /* Function */
    owobuiltin builtin;
    owoenv * env;
    owoval * formals;
    owoval * body;
    
    /* Expression
     * Count and Pointer to a list of "owoval*"
     */
    int count;
    owoval ** cell;
} owoval;

/**
 Create Enumeration of Possible owoval Types
 */
enum { OwOVAL_ERR, OwOVAL_NUM, OwOVAL_SYM, OwOVAL_STR, OwOVAL_SEXPR, OwOVAL_QEXPR, OwOVAL_FUNC };

/**
 Create Enumeration of Possible Error Types
 */
enum { OwOERR_DIV_ZERO, OwOERR_BAD_OP, OwOERR_BAD_NUM };

const char * owotype_name(int t);

/**
 *  @brief  Create a new error type owoval with message
 *
 *  @param x error message
 *
 *  @return owoval
 */
owoval * owoval_err(const char* fmt, ...);

/**
 *  @brief  Create a new number type owoval
 *
 *  @param x a number
 *
 *  @return owoval
 */
owoval * owoval_num(double x);

/**
 *  @brief  Create a new value type owoval
 *
 *  @param m mpf_class number
 *
 *  @return owoval
 */
//owoval * owoval_value(mpf_class& m);

/**
 *  @brief  Create a new value type owoval
 *
 *  @param x string type number
 *
 *  @return owoval
 */
//owoval * owoval_value(const char * x);

/**
 *  @brief  Construct a pointer to a new Symbol owoval
 *
 *  @param s symbol
 *
 *  @return owoval
 */
owoval * owoval_sym(const char * s);

/**
 *  @brief  Construct a pointer to a new string owoval
 *
 *  @param s string
 *
 *  @return owoval
 */
owoval * owoval_str(const char * s);

/**
 *  @brief  A pointer to a new empty Sexpr owoval
 *
 *  @return owoval
 */
owoval * owoval_sexpr(void);

/**
 *  @brief  A pointer to a new empty Qexpr owoval
 *
 *  @return owoval
 */
owoval * owoval_qexpr(void);

/**
 *  @brief  Read expression
 *
 *  @param t ast
 *
 *  @return owoval
 */
owoval * owoval_read(mpc_ast_t * t);

/**
 *  @brief  Read number type owoval expression
 *
 *  @param t ast
 *
 *  @return owoval
 */
owoval * owoval_read_num(mpc_ast_t * t);

/**
 *  @brief  Read string type owoval expression
 *
 *  @param t ast
 *
 *  @return owoval
 */
owoval * owoval_read_str(mpc_ast_t * t);

owoval * owoval_lambda(owoval * formals, owoval * body);

owoval * owoval_builtin(owobuiltin func);

owoval * owoval_add(owoval * v, owoval * x);

/**
 *  @brief  Print an "owoval"
 *
 *  @param v an owoval to print
 */
void owoval_print(owoval * v);

/**
 *  @brief  Print an "owoval" followed by a newline
 *
 *  @param v an owoval to print
 */
void owoval_println(owoval * v);

/**
 *  @brief  Print a string type "owoval"
 *
 *  @param v an owoval to print
 */
void owoval_print_str(owoval * v);

/**
 *  @brief  Print the Sexpr/Qexpr type "owoval"
 *
 *  @param v an owoval to print
 */
void owoval_expr_print(owoval * v, char open, char close);

owoval * owoval_eval(owoenv * e, owoval * v, bool debug=false);

owoval * owoval_eval_sexpr(owoenv * e, owoval * v, bool debug=false);

owoval * owoval_copy(owoval * v);

owoval * owoval_pop(owoval * v, int i);

owoval * owoval_take(owoval * v, int i);

owoval * owoval_join(owoval * x, owoval * y);

owoval * owoval_call(owoenv* e, owoval * f, owoval * a);

void owoval_del(owoval * v);

int owoval_eq(owoval * x, owoval * y);

#endif /* OWOVAL_H */
