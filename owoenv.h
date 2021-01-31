//
//  owoenv.hpp
//  OwO
//
//  Created by Ryza on 15/11/13.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#ifndef OWOENV_H
#define OWOENV_H

#include <stdio.h>
#include "owolang.h"

/**
 *  @brief  Forward Declarations
 */
struct owoval;
struct owoenv;
typedef owoval * (*owobuiltin)(owoenv *, owoval *);

typedef struct owoenv {
    /**
     *  @brief  Parent env
     */
    owoenv * par;
    
    /**
     *  @brief  Variables count
     */
    int count;
    
    /**
     *  @brief  Variable symbols
     */
    char ** syms;
    
    /**
     *  @brief  Variables
     */
    owoval ** vals;
} owoenv;

/**
 *  @brief  Construct a pointer to a new owoenv
 *
 *  @return owoenv
 */
owoenv * owoenv_new(void);

/**
 *  @brief  Define a variable in e
 *
 *  @discussion  This variable would be defined at the root node of e
 *
 *  @return owoenv
 */
void owoenv_def(owoenv * e, owoval * k, owoval * v);

/**
 *  @brief  Add a builtin function for a variable
 *
 *  @param e    env
 *  @param name symbol
 *  @param func builtin function
 */
void owoenv_add_builtin(owoenv * e, const char* name, owobuiltin func);

/**
 *  @brief  Put a variable in e
 *
 *  @param e env
 *  @param k symbol anme
 *  @param v variable
 */
void owoenv_put(owoenv * e, owoval * k, owoval * v);

/**
 *  @brief  Get a variable in e
 *
 *  @discussion  If this symbol isn't in this env, we'll check the parent node of the given env
 *
 *  @param e env
 *  @param k symbol name
 *
 *  @return A copy of that variable
 */
owoval * owoenv_get(owoenv * e, owoval * k);

/**
 *  @brief  Create a copy of the given env
 *
 *  @param e env
 *
 *  @return A copy of the given env
 */
owoenv * owoenv_copy(owoenv * e);

/**
 *  @brief  Delete the env
 *
 *  @param e env
 */
void owoenv_del(owoenv * e);

#endif /* OUOENV_H */
