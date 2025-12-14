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

#ifndef BUF_H
#define BUF_H

#include <stddef.h>

typedef struct buf *Buf;

/* Function declarations */
void free_buf(Buf b);

Buf init_buf(size_t init_num_elements, size_t element_size);

int push(Buf b, void *object);

int pop(Buf b, void *result);

void truncate_buf(Buf b);

void *get_buf_element(Buf b, size_t element);

size_t buf_num_used_elements(Buf b);

#endif
