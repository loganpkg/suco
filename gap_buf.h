/*
 * Copyright (c) 2025 Logan Ryan McLintock. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef GAP_BUF_H
#define GAP_BUF_H

#include <stddef.h>

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

char *gb_before_gap(Gap_buf gb, size_t *size);

char *gb_after_gap(Gap_buf gb, size_t *size);

#endif
