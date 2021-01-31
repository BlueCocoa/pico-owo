//
//  owobuiltin.h
//  OwO
//
//  Created by BlueCocoa on 15/11/13.
//  Copyright © 2015 OwOlang. All rights reserved.
//

#ifndef OWOBUILTIN_H
#define OWOBUILTIN_H

#include <stdio.h>
#include "owolang.h"

/**
 *  @brief  Forward Declarations
 */
struct owoval;
struct owoenv;
typedef owoval * (*owobuiltin)(owoenv *, owoval *);

/**
 *  @brief  <#Description#>
 *
 *  @param e    <#e description#>
 *  @param a    <#a description#>
 *  @param func <#func description#>
 *
 *  @return <#return value description#>
 */
owoval * builtin_var(owoenv * e, owoval * a, const char * func);

/**
 *  @brief  Load a file
 *
 *  @param e env
 *  @param a An Sexpr with file path
 *
 *  @return Result of evaluation
 */

owoval * builtin_load(owoenv * e, owoval * a);

owoval * builtin_error(owoenv * e, owoval * a);

owoval * builtin_print(owoenv * e, owoval * a);

owoval * builtin_println(owoenv * e, owoval * a);

owoval * builtin_lambda(owoenv * e, owoval * a);

owoval * builtin_def(owoenv * e, owoval * a);

owoval * builtin_put(owoenv * e, owoval * a);

owoval * builtin_con(owoenv *e, owoval * a);

/* new
 */
//owoval * builtin_new(owoenv *e, owoval * a);

owoval * builtin_if(owoenv * e, owoval* a);

owoval * builtin_eq(owoenv * e, owoval * a);

owoval * builtin_ne(owoenv * e, owoval * a);

owoval * builtin_gt(owoenv * e, owoval * a);

owoval * builtin_lt(owoenv * e, owoval * a);

owoval * builtin_ge(owoenv * e, owoval * a);

owoval * builtin_le(owoenv * e, owoval * a);

owoval * builtin_cmp(owoenv * e, owoval * a, const char * op);

owoval * builtin_ord(owoenv * e, owoval * a, const char* op);

/* Use operator string to see which operation to perform */
owoval * builtin_op(owoenv * e, owoval * a, const char * op);

owoval * builtin_eval(owoenv * e, owoval * a);

owoval * builtin_list(owoenv * e, owoval * a);

owoval * builtin_head(owoenv * e, owoval * a);

owoval * builtin_tail(owoenv * e, owoval * a);

owoval * builtin_eval(owoenv * e, owoval * a);

owoval * builtin_join(owoenv * e, owoval * a);

owoval * builtin_add(owoenv * e, owoval * a);

owoval * builtin_sub(owoenv * e, owoval * a);

owoval * builtin_mul(owoenv * e, owoval * a);

owoval * builtin_div(owoenv * e, owoval * a);

owoval * builtin_power(owoenv * e, owoval * a);

owoval * builtin_mod(owoenv * e, owoval * a);

owoval * builtin_in(owoenv * e, owoval * a);

owoval * builtin_loop(owoenv * e, owoval * a);

// owoval * builtin_getcwd(owoenv * e, owoval * a);

// owoval * builtin_import(owoenv * e, owoval * a);

#endif /* OWOBUILTIN_H */
