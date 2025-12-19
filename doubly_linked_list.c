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

#include "debug.h"
#include "doubly_linked_list.h"


int dll_add_node(Dlln *p, void *data)
{
    Dlln n = NULL;

    if (p == NULL)
        debug(return 1);

    if ((n = calloc(1, sizeof(struct dll_node))) == NULL)
        debug(return 1);

    n->data = data;

    if (*p != NULL) {
        /* Link in on the left. */
        n->prev = (*p)->prev;
        n->next = *p;
        (*p)->prev = n;
    } else {
        n->prev = NULL;
        n->next = NULL;
    }

    /* Update pointer. */
    *p = n;
    return 0;
}


int free_dll_node(Dlln *p, Free_data_func fdf)
{
    Dlln t = NULL;
    int r = 0;

    if (p == NULL)
        debug(return 1);

    if (*p == NULL)
        return 0;

    /* Link around. */
    if ((*p)->prev != NULL) {
        t = (*p)->prev;
        (*p)->prev->next = (*p)->next;
    }

    if ((*p)->next != NULL) {
        if (t == NULL)
            t = (*p)->next;

        (*p)->next->prev = (*p)->prev;
    }

    r = (*fdf) ((*p)->data);

    free(*p);
    *p = t;
    return r;
}


int free_dll(Dlln *p, Free_data_func fdf)
{
    int r = 0;
    if (p == NULL)
        debug(return 1);

    while (*p != NULL)
        if (free_dll_node(p, fdf))
            r = 1;

    return r;
}
