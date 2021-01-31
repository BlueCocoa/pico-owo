//
//  owostring.cpp
//  OwO
//
//  Created by Ryza on 15/11/16.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#include "owopico.h"
#include "pico/stdlib.h"

owoval * builtin_gpio_init(owoenv * e, owoval * a) {
    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        int flag = 1;
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

    /* While there are still elements remaining */
    while (a->count > 0) {
        /* Pop the top element */
        owoval * pin = owoval_pop(a, 0);
        gpio_init((int)pin->num);
        owoval_del(pin);
    }
    owoval_del(a);
    return owoval_sexpr();
}

owoval * builtin_gpio_set_dir(owoenv * e, owoval * a) {
    if (a->count <= 1) {
        OwOASSERT(a, 0, "[gpio_set_dir \"out\" 25 26]");
    } else {
        owoval * dir = owoval_pop(a, 0);
        int flag = 0;
        int is_out = 0;
        if (dir->type == OwOVAL_QEXPR || dir->type == OwOVAL_STR) {
            is_out = strcmp(dir->str, "in");
            if (is_out == 0 || strcmp(dir->str, "out") == 0) {
                flag = 1;
            }
        }
        owoval_del(dir);

        if (flag == 0) {
            OwOASSERT(a, 0, "first param of gpio_set_dir should be a str, \"in\" or \"out\"");
        } else {
            /* Ensure rest arguments are numbers */
            for (int i = 1; i < a->count; i++) {
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

            /* While there are still elements remaining */
            while (a->count > 0) {
                /* Pop the top element */
                owoval * pin = owoval_pop(a, 0);
                gpio_set_dir((int)pin->num, is_out ? GPIO_OUT : GPIO_IN);
                owoval_del(pin);
            }
        }
    }
    owoval_del(a);
    return owoval_sexpr();
}

owoval * builtin_gpio_put(owoenv * e, owoval * a) {
    if (a->count <= 1) {
        OwOASSERT(a, 0, "[gpio_put 1 25 26]\n[gpio_put 0 25 26]");
        owoval_del(a);
        return owoval_sexpr();
    }

    /* Ensure all arguments are numbers */
    for (int i = 0; i < a->count; i++) {
        int flag = 1;
        if (a->cell[i]->type == OwOVAL_QEXPR || a->cell[i]->type == OwOVAL_NUM) {
            flag = 1;
        }
        OwOASSERT(a, flag, "Wrong data type.")
    }
    
    if (a->cell[0]->type == OwOVAL_QEXPR) {
        a = a->cell[0];
        a->type = OwOVAL_NUM;
    }

    owoval * val = owoval_pop(a, 0);
    int value = (int)val->num;
    owoval_del(val);

    /* While there are still elements remaining */
    while (a->count > 0) {
        /* Pop the top element */
        owoval * pin = owoval_pop(a, 0);
        gpio_put((int)pin->num, value);
        owoval_del(pin);
    }

    owoval_del(a);
    return owoval_sexpr();
}

owoval * builtin_sleep_ms(owoenv * e, owoval * a) {
    if (a->count != 1) {
        OwOASSERT(a, 0, "[sleep_ms 100]");
        owoval_del(a);
        return owoval_sexpr();
    }

    if (a->cell[0]->type == OwOVAL_QEXPR && !a->cell[0]->type == OwOVAL_NUM) {
        OwOASSERT(a, 0, "sleep_ms expects a number");
        owoval_del(a);
        return owoval_sexpr();
    }

    if (a->cell[0]->type == OwOVAL_QEXPR) {
        a = a->cell[0];
        a->type = OwOVAL_NUM;
    }

    owoval * time = owoval_pop(a, 0);
    int value = (int)time->num;
    sleep_ms(value);
    owoval_del(time);
    owoval_del(a);
    return owoval_sexpr();
}
