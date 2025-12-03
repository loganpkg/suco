/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#include <stdint.h>
#include "int.h"

int mult_overflow(size_t a, size_t b)
{
    if (b && a > SIZE_MAX / b)
        return 1;

    return 0;
}
