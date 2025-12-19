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

#include <debug.h>
#include <doubly_linked_list.h>


void print_this_and_next_data(Dlln n)
{
    printf("n->data: %s\n", n != NULL ? (char *) n->data : "NULL");
    printf("n->next->data: %s\n\n", n != NULL
           && n->next != NULL ? (char *) n->next->data : "NULL");
}


int add_node_with_str(Dlln *n, char *str)
{
    size_t len;
    char *t;

    if (str == NULL)
        debug(return 1);

    len = strlen(str);

    if ((t = calloc(len + 1, sizeof(char))) == NULL)
        debug(return 1);

    memmove(t, str, len + 1);

    if (dll_add_node(n, t)) {
        free(t);
        debug(return 1);
    }

    return 0;
}


int custom_free(void *data)
{
    free(data);
    return 0;
}


int main(void)
{
    Dlln n = NULL;

    print_this_and_next_data(n);

    if (add_node_with_str(&n, "hello"))
        debug(goto error);

    print_this_and_next_data(n);

    if (add_node_with_str(&n, "world"))
        debug(goto error);

    print_this_and_next_data(n);

    if (add_node_with_str(&n, "elephant"))
        debug(goto error);

    print_this_and_next_data(n);

    if (add_node_with_str(&n, "wow"))
        debug(goto error);

    print_this_and_next_data(n);

    if (free_dll_node(&n, &custom_free))
        debug(goto error);

    print_this_and_next_data(n);

    if (add_node_with_str(&n, "goat"))
        debug(goto error);

    print_this_and_next_data(n);

    return free_dll(&n, &custom_free);

  error:
    free_dll(&n, &custom_free);
    debug(return 1);
}
