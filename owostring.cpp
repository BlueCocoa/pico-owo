//
//  owostring.cpp
//  OwO
//
//  Created by Ryza on 15/11/16.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#include "owoval.h"
#include "owostring.h"

owoval * len (owoenv * e, owoval * a) {
    OwOASSERT(a, a->count == 1, "Too many arguments have passed when I just need ONE!");
    
    return owoval_num(strlen(a->cell[0]->str));
}
owoval * cmp (owoenv * e, owoval * a) {
    OwOASSERT(a, a->count >= 2, "What the fuck you want me to compare when I only have one argument?");
    OwOASSERT(a, a->count <= 2, "Too many arguments have passed when I just need TWO!");
    
    return owoval_num(strcmp(a->cell[0]->str, a->cell[1]->str));
}
owoval * strn (owoenv * e, owoval * a) {
    OwOASSERT(a, a->count, "Two many or only one argument to fuck up!");
    OwOASSERT(a, a->cell[0]->type == OwOVAL_STR /* && the type of value */, "Wrong argument type You DICKSUCKINGCONDOMSTER!!!");
    
    
    char tmp = a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)];
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = '\0';
    owoval *res = owoval_str(&a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)]);
    a->cell[0]->str[(int)(a->cell[1]->num + a->cell[2]->num)] = tmp;
    return owoval_str(res->str);
}

owoval * endswith(owoenv * e, owoval * a) {
    OwOASSERT(a, (a->count & 0x1) == 0, "endswith function needs even number arguments");
    
    owoval * r = owoval_qexpr();
    
    for (int i = 0; i < a->count; i += 2) {
        owoval * arg1 = a->cell[i];
        OwOASSERT(a, a->cell[i + 1]->type == OwOVAL_STR /* && the type of value */, "Wrong argument type for argument at %d", i + 1);
        
        
        char * ending = a->cell[i + 1]->str;
        
        size_t ending_len = strlen(ending) - 1;
        
        if (arg1->type == OwOVAL_STR) {
            char * string = arg1->str;
            size_t string_len = strlen(string) - 1;
            
            if (string_len < ending_len) {
                r = owoval_join(r, owoval_num(0));
            } else {
                int found = 1;
                for (long long pos = 0; pos <= ending_len; pos++) {
                    if (ending[ending_len - pos] != string[string_len - pos]) {
                        found = 0;
                        break;
                    }
                }
                r = owoval_add(r, owoval_num(found));
            }
        } else if (arg1->type == OwOVAL_QEXPR) {
            for (int t = 0; t < arg1->count; t++) {
                if (arg1->cell[t]->type != OwOVAL_STR) {
                    owoval_del(r);
                    return owoval_err("Argument %d, index %d must be a string", i, t);
                }
                char * string = arg1->cell[t]->str;
                size_t string_len = strlen(string) - 1;
                
                if (string_len < ending_len) {
                    r = owoval_join(r, owoval_num(0));
                } else {
                    int found = 1;
                    for (long long pos = 0; pos <= ending_len; pos++) {
                        if (ending[ending_len - pos] != string[string_len - pos]) {
                            found = 0;
                            break;
                        }
                    }
                    r = owoval_add(r, owoval_num(found));
                }
            }
        }
    }
    return r;
}

owoval * hash (owoenv * e, owoval * a) {
    uint64_t hash = 0;
    uint64_t i;
    
    for (i = 0; i < strlen(a->cell[0]->str) ; i++) {
        hash += (uint64_t)a->cell[0]->str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    printf("Debug:: %lld\n", hash);
    return owoval_num(hash);
}
