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
