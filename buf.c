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

#include <stdlib.h>
#include <string.h>

#include "buf.h"
#include "int.h"

struct buf {
    void *a;                    /* Memory. */
    size_t i;                   /* Index of next unused element. */
    size_t n;                   /* Number of elements allocated. */
    size_t es;                  /* Element size. */
};

Buf init_buf(size_t init_num_elements, size_t element_size)
{
    Buf t;

    if (!init_num_elements || !element_size)
        return NULL;

    if ((t = calloc(1, sizeof(struct buf))) == NULL)
        return NULL;

    if (mult_overflow(init_num_elements, element_size)) {
        free(t);
        return NULL;
    }

    if ((t->a = calloc(init_num_elements, element_size)) == NULL) {
        free(t);
        return NULL;
    }

    t->n = init_num_elements;
    t->es = element_size;

    return t;
}

void free_buf(Buf b)
{
    if (b != NULL) {
        free(b->a);
        free(b);
    }
}

int push(Buf b, void *object)
{
    /* Assumes object is of size b->es. */
    size_t new_n;
    void *t;

    if (b->i == b->n) {
        /* Need to grow the buffer. */
        if (mult_overflow(b->n, 2))
            return 1;

        new_n = b->n * 2;

        if (mult_overflow(new_n, b->es))
            return 1;

        if ((t = realloc(b->a, new_n * b->es)) == NULL)
            return 1;

        b->a = t;
        b->n = new_n;
    }

    memmove((char *) b->a + b->i++ * b->es, object, b->es);

    return 0;
}

int pop(Buf b, void *result)
{
    if (!b->i)
        return 1;

    memmove(result, (char *) b->a + --b->i * b->es, b->es);

    return 0;
}
