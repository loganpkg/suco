/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#include <stdio.h>

#include "../debug.h"
#include "../gap_buf.h"


#define INIT_NUM_ELEMENTS 10


int main(void)
{
    Gap_buf gb;

    if ((gb = init_gap_buf(INIT_NUM_ELEMENTS)) == NULL)
        debug(goto error);

    print_gap_buf(gb);

    printf("Insert some characters:\n");

    if (insert_ch(gb, 'e'))
        debug(goto error);

    if (insert_ch(gb, 'l'))
        debug(goto error);

    if (insert_ch(gb, 'e'))
        debug(goto error);

    if (insert_ch(gb, 'p'))
        debug(goto error);

    if (insert_ch(gb, 'h'))
        debug(goto error);

    if (insert_ch(gb, 'a'))
        debug(goto error);

    if (insert_ch(gb, 'n'))
        debug(goto error);

    if (insert_ch(gb, 't'))
        debug(goto error);

    print_gap_buf(gb);

    printf("Move left 4 times:\n");

    if (left_ch(gb))
        debug(goto error);

    if (left_ch(gb))
        debug(goto error);

    if (left_ch(gb))
        debug(goto error);

    if (left_ch(gb))
        debug(goto error);

    print_gap_buf(gb);

    printf("Delete two characters:\n");

    if (delete_ch(gb))
        debug(goto error);

    if (delete_ch(gb))
        debug(goto error);

    print_gap_buf(gb);

    printf("Undo once:\n");

    if (undo_gap_buf(gb))
        debug(goto error);

    print_gap_buf(gb);

    printf("Redo once:\n");

    if (redo_gap_buf(gb))
        debug(goto error);

    print_gap_buf(gb);

    printf("Insert a file:\n");

    if (insert_file(gb, "README.md"))
        debug(goto error);

    print_gap_buf(gb);

    printf("Undo:\n");

    if (undo_gap_buf(gb))
        debug(goto error);

    print_gap_buf(gb);

    printf("Redo:\n");

    if (redo_gap_buf(gb))
        debug(goto error);

    print_gap_buf(gb);

    free_gap_buf(gb);
    return 0;

  error:
    free_gap_buf(gb);
    debug(return 1);
}
