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

int main(void)
{
    Buf b;
    char str[10];

    if ((b = init_buf(1, 10)) == NULL)
        return 1;

    if (push(b, "cool")) {
        free_buf(b);
        return 1;
    }

    if (push(b, "elephant!")) {
        free_buf(b);
        return 1;
    }

    if (push(b, "whale")) {
        free_buf(b);
        return 1;
    }

    if (pop(b, str)) {
        free_buf(b);
        return 1;
    }

    printf("%s\n", str);

    if (pop(b, str)) {
        free_buf(b);
        return 1;
    }

    printf("%s\n", str);

    if (pop(b, str)) {
        free_buf(b);
        return 1;
    }

    printf("%s\n", str);

    free_buf(b);
    return 0;
}
