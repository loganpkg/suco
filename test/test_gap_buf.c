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
