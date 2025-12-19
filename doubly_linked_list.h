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

#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H


/* Doubly linked list node. */
typedef struct dll_node *Dlln;


struct dll_node {
    void *data;
    Dlln prev;
    Dlln next;
};


typedef int (*Free_data_func)(void *);


/* Function declarations */
int dll_add_node(Dlln *p, void *data);

int free_dll_node(Dlln *p, Free_data_func fdf);

int free_dll(Dlln *p, Free_data_func fdf);

#endif
