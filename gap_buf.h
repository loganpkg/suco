/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#ifndef GAP_BUF_H
#define GAP_BUF_H

#include <stddef.h>

typedef struct gap_buf *Gap_buf;


/* Function declarations */
void free_gap_buf(Gap_buf gb);

Gap_buf init_gap_buf(size_t init_num_elements);

int insert_ch(Gap_buf gb, char ch);

int delete_ch(Gap_buf gb);

int left_ch(Gap_buf gb);

int right_ch(Gap_buf gb);

int undo_gap_buf(Gap_buf gb);

int redo_gap_buf(Gap_buf gb);

void print_gap_buf(Gap_buf gb);

int insert_file(Gap_buf gb, const char *fn);

#endif
