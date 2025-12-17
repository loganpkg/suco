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

#include "screen.h"

typedef struct gap_buf *Gap_buf;


/* Function declarations */
void free_gap_buf(Gap_buf gb);

Gap_buf init_gap_buf(size_t init_num_elements);

int gb_insert_ch(Gap_buf gb, char ch);

int gb_delete_ch(Gap_buf gb);

int gb_left_ch(Gap_buf gb);

int gb_right_ch(Gap_buf gb);

int gb_undo(Gap_buf gb);

int gb_redo(Gap_buf gb);

void gb_debug_print(Gap_buf gb);

int gb_insert_file(Gap_buf gb, const char *fn);

int gb_set_fn(Gap_buf gb, const char *fn);

void gb_start_of_line(Gap_buf gb);

void gb_end_of_line(Gap_buf gb);

void gb_set_mark(Gap_buf gb);

void gb_request_centring(Gap_buf gb);

int gb_print(Gap_buf gb, Screen sc, size_t y_origin, size_t x_origin,
             size_t sub_h, size_t sub_w, int move_cursor);

#endif
