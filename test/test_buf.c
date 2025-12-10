/*
 * Copyright (c) 2025 Logan Ryan McLintock. All rights reserved.
 *
 * The contents of this file are subject to the
 * Common Development and Distribution License (CDDL) version 1.1
 * (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License in
 * the LICENSE file included with this software or at
 * https://opensource.org/license/cddl-1-1
 *
 * NOTICE PURSUANT TO SECTION 4.2 OF THE LICENSE:
 * This software is prohibited from being distributed or otherwise made
 * available under any subsequent version of the License.
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
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
