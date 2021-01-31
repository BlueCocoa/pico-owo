//
//  owoenv.cpp
//  OwO
//
//  Created by Ryza on 15/11/13.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#include "owoenv.h"
#include "owoval.h"
#include "pico/stdlib.h"
#include <cstdlib>
#include <string.h>

owoenv * owoenv_new(void) {
    owoenv * e = (owoenv *)malloc(sizeof(owoenv));
    e->par = NULL;
    e->count = 0;
    e->syms = NULL;
    e->vals = NULL;
    return e;
}

void owoenv_def(owoenv * e, owoval * k, owoval * v) {
    /* Iterate till e has no parent */
    while (e->par) { e = e->par; }
    /* Put value in e */
    owoenv_put(e, k, v);
}

void owoenv_add_builtin(owoenv * e, const char* name, owobuiltin func) {
    owoval * k = owoval_sym(name);
    owoval * v = owoval_builtin(func);
    owoenv_put(e, k, v);
    owoval_del(k); owoval_del(v);
}

void owoenv_put(owoenv * e, owoval * k, owoval * v) {
    
    /* Iterate over all items in environment */
    /* This is to see if variable already exists */
    for (int i = 0; i < e->count; i++) {
        
        /* If variable is found delete item at that position */
        /* And replace with variable supplied by user */
        if (strcmp(e->syms[i], k->sym) == 0) {
            owoval_del(e->vals[i]);
            e->vals[i] = owoval_copy(v);
            return;
        }
    }
    
    /* If no existing entry found allocate space for new entry */
    e->count++;
    e->vals = (owoval **)realloc(e->vals, sizeof(owoval *) * e->count);
    e->syms = (char **)realloc(e->syms, sizeof(char *) * e->count);
    
    /* Copy contents of lval and symbol string into new location */
    e->vals[e->count-1] = owoval_copy(v);
    e->syms[e->count-1] = (char *)malloc(strlen(k->sym)+1);
    strcpy(e->syms[e->count-1], k->sym);
}

owoval * owoenv_get(owoenv * e, owoval * k) {
    
    /* Iterate over all items in environment */
    for (int i = 0; i < e->count; i++) {
        /* Check if the stored string matches the symbol string */
        /* If it does, return a copy of the value */
        if (strcmp(e->syms[i], k->sym) == 0) {
            return owoval_copy(e->vals[i]);
        }
    }
    /* If no symbol check in parent otherwise error */
    if (e->par) {
        return owoenv_get(e->par, k);
    }
    return owoval_err("unbound symbol!");
}

owoenv * owoenv_copy(owoenv * e) {
    owoenv * n = (owoenv *)malloc(sizeof(owoenv));
    n->par = e->par;
    n->count = e->count;
    n->syms = (char **)malloc(sizeof(char *) * n->count);
    n->vals = (owoval **)malloc(sizeof(owoval *) * n->count);
    for (int i = 0; i < e->count; i++) {
        n->syms[i] = (char *)malloc(strlen(e->syms[i]) + 1);
        strcpy(n->syms[i], e->syms[i]);
        n->vals[i] = owoval_copy(e->vals[i]);
    }
    return n;
}

void owoenv_del(owoenv * e) {
    for (int i = 0; i < e->count; i++) {
        free(e->syms[i]);
        owoval_del(e->vals[i]);
    }
    free(e->syms);
    free(e->vals);
    free(e);
}
