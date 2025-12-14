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
#include <stdlib.h>
#include <string.h>

#include "buf.h"
#include "debug.h"
#include "int.h"


struct buf {
    char *a;                    /* Memory. */
    size_t i;                   /* Index of next unused element. */
    size_t n;                   /* Number of elements allocated. */
    size_t es;                  /* Element size. */
};


void free_buf(Buf b)
{
    if (b != NULL) {
        free(b->a);
        free(b);
    }
}


Buf init_buf(size_t init_num_elements, size_t element_size)
{
    Buf b = NULL;

    if (!init_num_elements || !element_size)
        debug(goto error);

    if ((b = calloc(1, sizeof(struct buf))) == NULL)
        debug(goto error);

    b->a = NULL;                /* Do not assume that NULL is zero. */

    if (mult_overflow(init_num_elements, element_size))
        debug(goto error);

    if ((b->a = calloc(init_num_elements, element_size)) == NULL)
        debug(goto error);

    b->n = init_num_elements;
    b->es = element_size;

    return b;

  error:
    free_buf(b);
    debug(return NULL);
}


int push(Buf b, void *object)
{
    /* Assumes object is of size b->es. */
    size_t new_n;
    void *t;

    if (b->i == b->n) {
        /* Need to grow the buffer. */
        if (mult_overflow(b->n, 2))
            debug(return 1);

        new_n = b->n * 2;

        if (mult_overflow(new_n, b->es))
            debug(return 1);

        if ((t = realloc(b->a, new_n * b->es)) == NULL)
            debug(return 1);

        b->a = t;
        b->n = new_n;
    }

    memmove(b->a + b->i++ * b->es, object, b->es);

    return 0;
}


int pop(Buf b, void *result)
{
    if (!b->i)
        return 1;               /* Only returns 1 when empty. */

    memmove(result, b->a + --b->i * b->es, b->es);

    return 0;
}


void truncate_buf(Buf b)
{
    b->i = 0;
}


void *get_buf_element(Buf b, size_t element)
{
    /* Pointer can change after reallocation, so not safe to use after push. */
    if (element >= b->i)
        debug(return NULL);     /* Out of bounds. */

    return b->a + element * b->es;
}

size_t buf_num_used_elements(Buf b)
{
    return b->i;
}
