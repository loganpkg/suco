/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#include <stdio.h>

#include "../buf.h"
#include "../debug.h"


#define INIT_NUM_ELEMENTS 1
#define ELEMENT_SIZE 10


int main(void)
{
    Buf b = NULL;
    char str[ELEMENT_SIZE];

    if ((b = init_buf(INIT_NUM_ELEMENTS, ELEMENT_SIZE)) == NULL)
        debug(goto error);

    if (push(b, "cool"))
        debug(goto error);

    if (push(b, "elephant!"))
        debug(goto error);

    if (push(b, "whale"))
        debug(goto error);

    if (pop(b, str))
        debug(goto error);

    printf("%s\n", str);

    if (pop(b, str))
        debug(goto error);

    printf("%s\n", str);

    if (pop(b, str))
        debug(goto error);

    printf("%s\n", str);

    free_buf(b);
    return 0;

  error:
    free_buf(b);
    debug(return 1);
}
