//
//  owostring.h
//  OwO
//
//  Created by Ryza on 15/11/16.
//  Copyright © 2021 OwOlang. All rights reserved.
//

#ifndef OWOSTRING_H
#define OWOSTRING_H

#include <stdio.h>
#include <string.h>
#include "owolang.h"

owoval * len (owoenv * e, owoval * a);
owoval * cmp (owoenv * e, owoval * a);
owoval * strn (owoenv * e, owoval * a);
owoval * endswith(owoenv * e, owoval * a);
owoval * hash (owoenv * e, owoval * a);

#endif /* OWOSTRING_H */
