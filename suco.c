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

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "buf.h"
#include "debug.h"
#include "gap_buf.h"
#include "input.h"
#include "screen.h"


#define INIT_NUM_GB_BUF_ELEMENTS 12
#define INIT_NUM_GB_ELEMENTS 512

/* This moves the command identifiers beyond the normal cooked key range. */
#define CMD_ID_OFFSET 0x200

struct editor {
    Buf gb_buf;                 /* Buffer of gap buffers. */
    size_t active_gb_i;         /* Index into gb_buf to the active gb. */
    Gap_buf active_gb;          /* Shortcut. */
    Input ip;
    Screen sc;
    int rv;                     /* Return value of the last command. */
    int running;                /* Text editor is on. */
};


typedef struct editor *Editor;
typedef void (*Ed_func)(Editor);


int set_active_gb(Editor ed, size_t new_active_gb_i)
{
    Gap_buf *gb_p;

    if (new_active_gb_i >= buf_num_used_elements(ed->gb_buf))
        debug(return 1);

    if ((gb_p = get_buf_element(ed->gb_buf, new_active_gb_i)) == NULL)
        debug(return 1);

    ed->active_gb = *gb_p;
    ed->active_gb_i = new_active_gb_i;

    return 0;
}


int free_editor(Editor ed)
{
    Gap_buf gb;
    int r = 0;

    if (ed != NULL) {
        while (pop(ed->gb_buf, &gb) == 0) {
            if (gb_insert_ch(gb, 'c'))
                fprintf(stderr, "WOW\n");

            free_gap_buf(gb);
        }

        free_buf(ed->gb_buf);

        if (free_input(ed->ip))
            debug(r = 1);

        if (free_screen(ed->sc))
            debug(r = 1);

        free(ed);
    }
    return r;
}


Editor init_editor(const struct key_map *km)
{
    Editor ed = NULL;

    if ((ed = calloc(1, sizeof(struct editor))) == NULL)
        debug(goto error);

    ed->gb_buf = NULL;
    ed->ip = NULL;
    ed->sc = NULL;

    if ((ed->gb_buf =
         init_buf(INIT_NUM_GB_BUF_ELEMENTS, sizeof(Gap_buf))) == NULL)
        debug(goto error);

    if ((ed->ip = init_input_stdin(BLOCKING, DOUBLE_COOKED, km)) == NULL)
        debug(goto error);

    if ((ed->sc = init_screen()) == NULL)
        debug(goto error);

    ed->running = 1;

    return ed;

  error:
    free_editor(ed);
    debug(return NULL);
}


int add_gap_buf(Editor ed, const char *fn)
{
    Gap_buf gb = NULL;

    if ((gb = init_gap_buf(INIT_NUM_GB_ELEMENTS)) == NULL)
        debug(goto error);

    if (fn != NULL) {
        if (gb_insert_file(gb, fn))
            debug(goto error);

        if (gb_set_fn(gb, fn))
            debug(goto error);
    }

    if (push(ed->gb_buf, &gb))
        debug(goto error);

    return 0;

  error:
    free_gap_buf(gb);
    debug(return 1);
}


int draw_screen(Editor ed)
{
    Gap_buf gb = ed->active_gb;
    char *p;
    size_t s, i, y, x;

    if (clear_screen(ed->sc, SOFT_CLEAR))
        debug(return 1);

    if ((p = gb_before_gap(gb, &s)) == NULL)
        debug(return 1);

    for (i = 0; i < s; ++i)
        if (print_ch(ed->sc, *(p + i)))
            debug(return 1);

    /* Record location on the screen where the cursor should be. */
    y = get_y(ed->sc);
    x = get_x(ed->sc);

    if ((p = gb_after_gap(gb, &s)) == NULL)
        debug(return 1);

    for (i = 0; i < s; ++i)
        if (print_ch(ed->sc, *(p + i)))
            debug(return 1);

    /* Place cursor in the correct location. */
    if (move(ed->sc, y, x))
        debug(return 1);

    if (refresh_screen(ed->sc))
        debug(return 1);

    return 0;
}


void ed_delete_ch(Editor ed)
{
    ed->rv = gb_delete_ch(ed->active_gb);
}


void ed_left_ch(Editor ed)
{
    ed->rv = gb_left_ch(ed->active_gb);
}


void ed_right_ch(Editor ed)
{
    ed->rv = gb_right_ch(ed->active_gb);
}


void ed_undo(Editor ed)
{
    ed->rv = gb_undo(ed->active_gb);
}


void ed_redo(Editor ed)
{
    ed->rv = gb_redo(ed->active_gb);
}


void ed_start_of_line(Editor ed)
{
    gb_start_of_line(ed->active_gb);
}


void ed_end_of_line(Editor ed)
{
    gb_end_of_line(ed->active_gb);
}


void ed_close(Editor ed)
{
    ed->running = 0;
}


int main(int argc, char **argv)
{
    Editor ed = NULL;
    int i, ch;

    const struct key_map km[] = {
        { { CTRL_D}, CMD_ID_OFFSET },
        { { KEY_DELETE}, CMD_ID_OFFSET + 1 },
        { { CTRL_B}, CMD_ID_OFFSET + 2 },
        { { KEY_LEFT}, CMD_ID_OFFSET + 3 },
        { { CTRL_F}, CMD_ID_OFFSET + 4 },
        { { KEY_RIGHT}, CMD_ID_OFFSET + 5 },
        { { ESC, '-'}, CMD_ID_OFFSET + 6 },
        { { ESC, '='}, CMD_ID_OFFSET + 7 },
        { { CTRL_A}, CMD_ID_OFFSET + 8 },
        { { KEY_HOME}, CMD_ID_OFFSET + 9 },
        { { CTRL_E}, CMD_ID_OFFSET + 10 },
        { { KEY_END}, CMD_ID_OFFSET + 11 },
        { { CTRL_X, CTRL_C}, CMD_ID_OFFSET + 12 },
        { { 0}, 0 }
    };

    const Ed_func edf[] = {
        &ed_delete_ch,
        &ed_delete_ch,
        &ed_left_ch,
        &ed_left_ch,
        &ed_right_ch,
        &ed_right_ch,
        &ed_undo,
        &ed_redo,
        &ed_start_of_line,
        &ed_start_of_line,
        &ed_end_of_line,
        &ed_end_of_line,
        &ed_close
    };

    if ((ed = init_editor(km)) == NULL)
        debug(goto error);

    if (argc > 1) {
        for (i = 1; i < argc; ++i)
            if (add_gap_buf(ed, *(argv + i)))
                debug(goto error);
    } else {
        if (add_gap_buf(ed, NULL))
            debug(goto error);
    }

    if (set_active_gb(ed, 0))
        debug(goto error);

    while (ed->running) {
        if (draw_screen(ed))
            debug(goto error);

        /*
         * Clear the return value after being displayed.
         * This is important, as some functions do not set rv.
         */
        ed->rv = 0;

        if (get_ch(ed->ip, &ch))
            debug(goto error);

        if (ch >= CMD_ID_OFFSET)
            (*edf[ch - CMD_ID_OFFSET]) (ed);
        else if (ch <= UCHAR_MAX && isprint(ch)
                 && gb_insert_ch(ed->active_gb, ch))
            debug(goto error);
    }

    return free_editor(ed);

  error:
    free_editor(ed);
    debug(return 1);
}
