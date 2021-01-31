//
//  owoassert.h
//  OwO
//
//  Created by Ryza on 15/11/13.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#ifndef OWOASSERT_H
#define OWOASSERT_H

#define OwOASSERT(args, cond, fmt, ...) \
    if (!(cond)) { owoval* err = owoval_err(fmt, ##__VA_ARGS__); owoval_del(args); return err; }

#define OwOASSERT_TYPE(func, args, index, expect) \
    OwOASSERT(args, args->cell[index]->type == expect, \
    "Function '%s' passed incorrect type for argument %i. Got %s, Expected %s.", \
    func, index, owotype_name(args->cell[index]->type), owotype_name(expect))

#define OwOASSERT_NUM(func, args, num) \
    OwOASSERT(args, args->count == num, \
    "Function '%s' passed incorrect number of arguments. Got %i, Expected %i.", \
    func, args->count, num)

#define OwOASSERT_NOT_EMPTY(func, args, index) \
    OwOASSERT(args, args->cell[index]->count != 0, \
    "Function '%s' passed {} for argument %i.", func, index);

#endif /* OWOASSERT_H */
