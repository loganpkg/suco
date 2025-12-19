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

#include <debug.h>
#include <gap_buf.h>


#define INIT_NUM_ELEMENTS 10


int main(void)
{
    Gap_buf gb;

    if ((gb = init_gap_buf(INIT_NUM_ELEMENTS)) == NULL)
        debug(goto error);

    gb_debug_print(gb);

    printf("Insert some characters:\n");

    if (gb_insert_ch(gb, 'e'))
        debug(goto error);

    if (gb_insert_ch(gb, 'l'))
        debug(goto error);

    if (gb_insert_ch(gb, 'e'))
        debug(goto error);

    if (gb_insert_ch(gb, 'p'))
        debug(goto error);

    if (gb_insert_ch(gb, 'h'))
        debug(goto error);

    if (gb_insert_ch(gb, 'a'))
        debug(goto error);

    if (gb_insert_ch(gb, 'n'))
        debug(goto error);

    if (gb_insert_ch(gb, 't'))
        debug(goto error);

    gb_debug_print(gb);

    printf("Move left 4 times:\n");

    if (gb_left_ch(gb))
        debug(goto error);

    if (gb_left_ch(gb))
        debug(goto error);

    if (gb_left_ch(gb))
        debug(goto error);

    if (gb_left_ch(gb))
        debug(goto error);

    gb_debug_print(gb);

    printf("Delete two characters:\n");

    if (gb_delete_ch(gb))
        debug(goto error);

    if (gb_delete_ch(gb))
        debug(goto error);

    gb_debug_print(gb);

    printf("Undo once:\n");

    if (gb_undo(gb))
        debug(goto error);

    gb_debug_print(gb);

    printf("Redo once:\n");

    if (gb_redo(gb))
        debug(goto error);

    gb_debug_print(gb);

    printf("Insert a file:\n");

    if (gb_insert_file(gb, "README.md"))
        debug(goto error);

    gb_debug_print(gb);

    printf("Undo:\n");

    if (gb_undo(gb))
        debug(goto error);

    gb_debug_print(gb);

    printf("Redo:\n");

    if (gb_redo(gb))
        debug(goto error);

    gb_debug_print(gb);

    free_gap_buf(gb);
    return 0;

  error:
    free_gap_buf(gb);
    debug(return 1);
}
