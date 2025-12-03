/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#ifndef BUF_H
#define BUF_H

#include <stddef.h>

typedef struct buf *Buf;

/* Function declarations */
Buf init_buf(size_t init_num_elements, size_t element_size);
void free_buf(Buf b);
int push(Buf b, void *object);
int pop(Buf b, void *result);

#endif
