//
//  owopico.h
//  OwO
//
//  Created by Ryza on 21/01/30.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#ifndef OWOPICO_H
#define OWOPICO_H

#include <stdio.h>
#include "owolang.h"
#include "hardware/gpio.h"

// GPIO
owoval * builtin_gpio_init(owoenv * e, owoval * a);
owoval * builtin_gpio_set_dir(owoenv * e, owoval * a);
owoval * builtin_gpio_put(owoenv * e, owoval * a);

// function
owoval * builtin_sleep_ms(owoenv * e, owoval * a);

#endif /* OWOPICO_H */
