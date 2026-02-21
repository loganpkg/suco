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
#include "doubly_linked_list.c"
#include "gap_buf.h"
#include "input.h"
#include "screen.h"

#define INIT_NUM_GB_ELEMENTS 512

/* This moves the command identifiers beyond the normal cooked key range. */
#define CMD_ID_OFFSET 0x200

/* Increments each time it is used. */
#define CMD_COUNTER __COUNTER__

/* Command Identifier. */
#define ID (CMD_ID_OFFSET + CMD_COUNTER)

/* Split types: */
#define NO_SPLIT         1
#define VERTICAL_SPLIT   2
#define HORIZONTAL_SPLIT 4

/* Operation. 0 means no operation. */
#define ED_RENAME 1

/* Current gap buffer, excluding the cl. */
#define c_gb (ed->view_2 ? ed->n_2->data : ed->n->data)

/* Active gap buffer, including the cl. */
#define a_gb (ed->cl_a ? ed->cl : c_gb)

/*
 * full_clear:
 * 0 is the default, which does a soft clear of the sub screen,
 *     not the full screen.
 * SOFT_CLEAR is set when split or active buffer changes.
 * HARD_CLEAR is used when requested.
 */
struct editor {
    /* n and n_2 are nodes of a doubly linked list of gap buffers. */
    Dlln n;    /* Left or top view. */
    Dlln n_2;  /* Right or bottom view. Do not free. */
    int split; /* Split type. */
    int full_clear;
    int view_2;    /* n_2 is in use (cl_a could be set too). */
    Gap_buf cl;    /* Command line gap buffer. */
    int cl_a;      /* Command line is active. */
    int operation; /* The operation that using the cl. */
    Input ip;
    int ch; /* Read character. */
    Screen sc;
    int rv;      /* Return value of the last command. */
    int running; /* Text editor is on. */
};

typedef struct editor *Editor;
typedef void (*Ed_func)(Editor);

int free_dll_node_data(void *data)
{
    free_gap_buf(data);
    return 0;
}

int free_editor(Editor ed)
{
    int r = 0;

    if (ed != NULL) {
        if (free_dll(&ed->n, &free_dll_node_data))
            debug(r = 1);

        free_gap_buf(ed->cl);

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

    ed->n = NULL;
    ed->n_2 = NULL;
    ed->split = NO_SPLIT;
    ed->full_clear = SOFT_CLEAR;
    ed->cl = NULL;
    ed->ip = NULL;
    ed->sc = NULL;

    if ((ed->cl = init_gap_buf(INIT_NUM_GB_ELEMENTS)) == NULL)
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

    if (dll_add_node(&ed->n, gb))
        debug(goto error);

    return 0;

error:
    free_gap_buf(gb);
    debug(return 1);
}

int draw_screen(Editor ed)
{
    size_t h, w, y = 0, x = 0, y_2 = 0, x_2 = 0, cl_y = 0, cl_x = 0;

    if (ed->full_clear)
        if (clear_screen(ed->sc, ed->full_clear))
            debug(return 1);

    h = get_screen_height(ed->sc);
    w = get_screen_width(ed->sc);

    /* Check the minimum screen height. */
    if ((ed->split == HORIZONTAL_SPLIT && h < 5) || h < 3)
        debug(return 1);

    switch (ed->split) {
    case NO_SPLIT:
        if (ed->full_clear || !ed->cl_a) {
            if (!ed->full_clear)
                if (soft_clear_sub_screen(ed->sc, 0, 0, h - 1, w))
                    debug(return 1);

            if (gb_print(ed->n->data, ed->sc, 0, 0, h - 1, w,
                    INCLUDE_STATUS_BAR, &y, &x))
                debug(return 1);
        }

        break;
    case VERTICAL_SPLIT:
        if (ed->full_clear || (!ed->cl_a && !ed->view_2)) {
            if (!ed->full_clear)
                if (soft_clear_sub_screen(ed->sc, 0, 0, h - 1, w / 2))
                    debug(return 1);

            if (gb_print(ed->n->data, ed->sc, 0, 0, h - 1, w / 2,
                    INCLUDE_STATUS_BAR, &y, &x))
                debug(return 1);
        }

        if (ed->full_clear || (!ed->cl_a && ed->view_2)) {
            if (!ed->full_clear)
                if (soft_clear_sub_screen(ed->sc, 0, w / 2, h - 1, w / 2))
                    debug(return 1);

            if (gb_print(ed->n_2->data, ed->sc, 0, w / 2, h - 1, w / 2,
                    INCLUDE_STATUS_BAR, &y_2, &x_2))
                debug(return 1);
        }

        break;
    case HORIZONTAL_SPLIT:
        if (ed->full_clear || (!ed->cl_a && !ed->view_2)) {
            if (!ed->full_clear)
                if (soft_clear_sub_screen(ed->sc, 0, 0, h / 2, w))
                    debug(return 1);

            if (gb_print(ed->n->data, ed->sc, 0, 0, h / 2, w,
                    INCLUDE_STATUS_BAR, &y, &x))
                debug(return 1);
        }

        if (ed->full_clear || (!ed->cl_a && ed->view_2)) {
            if (!ed->full_clear)
                if (soft_clear_sub_screen(
                        ed->sc, h / 2, 0, h % 2 ? h / 2 : h / 2 - 1, w))
                    debug(return 1);

            if (gb_print(ed->n_2->data, ed->sc, h / 2, 0,
                    h % 2 ? h / 2 : h / 2 - 1, w, INCLUDE_STATUS_BAR, &y_2,
                    &x_2))
                debug(return 1);
        }

        break;
    default:
        debug(return 1); /* Invalid split type. */
    }

    if (ed->full_clear || ed->cl_a) {
        if (!ed->full_clear)
            if (soft_clear_sub_screen(ed->sc, h - 1, 0, 1, w))
                debug(return 1);

        if (gb_print(ed->cl, ed->sc, h - 1, 0, 1, w, EXCLUDE_STATUS_BAR, &cl_y,
                &cl_x))
            debug(return 1);
    }

    /* Position cursor on the display. */
    if (move(ed->sc, ed->cl_a ? cl_y : (ed->view_2 ? y_2 : y),
            ed->cl_a ? cl_x : (ed->view_2 ? x_2 : x)))
        debug(return 1);

    if (refresh_screen(ed->sc))
        debug(return 1);

    ed->full_clear = 0;

    return 0;
}

void ed_delete_ch(Editor ed)
{
    ed->rv = gb_delete_ch(a_gb);
}

void ed_left_ch(Editor ed)
{
    ed->rv = gb_left_ch(a_gb);
}

void ed_right_ch(Editor ed)
{
    ed->rv = gb_right_ch(a_gb);
}

void ed_undo(Editor ed)
{
    ed->rv = gb_undo(a_gb);
}

void ed_redo(Editor ed)
{
    ed->rv = gb_redo(a_gb);
}

void ed_start_of_line(Editor ed)
{
    gb_start_of_line(a_gb);
}

void ed_end_of_line(Editor ed)
{
    gb_end_of_line(a_gb);
}

void ed_close(Editor ed)
{
    ed->running = 0;
}

void ed_left_gb(Editor ed)
{
    if (ed->n->prev != NULL)
        ed->n = ed->n->prev;
}

void ed_right_gb(Editor ed)
{
    if (ed->n->next != NULL)
        ed->n = ed->n->next;
}

void ed_set_mark(Editor ed)
{
    gb_set_mark(a_gb);
}

void ed_centre(Editor ed)
{
    gb_request_centring(a_gb);
    ed->full_clear = HARD_CLEAR; /* Complete redraw. */
}

void ed_rename(Editor ed)
{
    ed->cl_a = 1;
    ed->operation = ED_RENAME;
}

void process_cl_operation(Editor ed)
{
    const char *cl_str;

    if ((cl_str = gb_to_str(ed->cl)) == NULL) {
        debug(ed->rv = 1);
        return;
    }

    switch (ed->operation) {
    case ED_RENAME:
        ed->rv = gb_set_fn(c_gb, cl_str);
        break;
    default:
        ed->rv = 1; /* Invalid operation. */
        return;
    }
    ed->operation = 0;
    ed->cl_a = 0;
}

void ed_toggle_split(Editor ed)
{
    if (ed->split == NO_SPLIT) {
        if (ed->n->next != NULL) {
            ed->n_2 = ed->n->next;
        } else if (ed->n->prev != NULL) {
            ed->n_2 = ed->n->prev;
        } else {
            /*
             * New nodes are linked in on the left,
             * and n is updated automatically,
             * so n_2 will be the next.
             */
            if (add_gap_buf(ed, NULL))
                debug(ed->rv = 1);

            ed->n_2 = ed->n;
            ed->n = ed->n->next;
        }
        ed->split = VERTICAL_SPLIT;
    } else if (ed->split == VERTICAL_SPLIT) {
        ed->split = HORIZONTAL_SPLIT;
    } else {
        if (ed->view_2)
            ed->n = ed->n_2;

        ed->n_2 = NULL;
        ed->view_2 = 0;
        ed->split = NO_SPLIT;
    }

    ed->full_clear = SOFT_CLEAR;
}

void ed_toggle_view(Editor ed)
{
    if (ed->n_2 == NULL)
        return;
    if (ed->view_2)
        ed->view_2 = 0;
    else
        ed->view_2 = 1;
}

int main(int argc, char **argv)
{
    Editor ed = NULL;
    int i;

    const struct key_map km[] = {
        { { CTRL_D }, ID },
        { { KEY_DELETE }, ID },
        { { CTRL_B }, ID },
        { { KEY_LEFT }, ID },
        { { CTRL_F }, ID },
        { { KEY_RIGHT }, ID },
        { { ESC, '-' }, ID },
        { { ESC, '=' }, ID },
        { { CTRL_A }, ID },
        { { KEY_HOME }, ID },
        { { CTRL_E }, ID },
        { { KEY_END }, ID },
        { { CTRL_X, CTRL_C }, ID },
        { { CTRL_X, KEY_LEFT }, ID },
        { { CTRL_LEFT }, ID },
        { { CTRL_X, KEY_RIGHT }, ID },
        { { CTRL_RIGHT }, ID },
        { { 0 }, ID },
        { { CTRL_L }, ID },
        { { ESC, '/' }, ID },
        { { ESC, 's' }, ID },
        { { ESC, 'v' }, ID },
        { { 0 }, 0 },
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
        &ed_close,
        &ed_left_gb,
        &ed_left_gb,
        &ed_right_gb,
        &ed_right_gb,
        &ed_set_mark,
        &ed_centre,
        &ed_rename,
        &ed_toggle_split,
        &ed_toggle_view,
    };

    if ((ed = init_editor(km)) == NULL)
        debug(goto error);

    if (argc > 1) {
        /* Do backwards, as nodes are linked in on the left. */
        for (i = argc - 1; i >= 1; --i)
            if (add_gap_buf(ed, *(argv + i)))
                debug(goto error);
    } else {
        if (add_gap_buf(ed, NULL))
            debug(goto error);
    }

    while (ed->running) {
        if (draw_screen(ed))
            debug(goto error);

        /*
         * Clear the return value after being displayed.
         * This is important, as some functions do not set rv.
         */
        ed->rv = 0;

        if (get_ch(ed->ip, &ed->ch))
            debug(goto error);

        if (ed->ch == '\r')
            ed->ch = '\n';

        if (ed->ch >= CMD_ID_OFFSET)
            (*edf[ed->ch - CMD_ID_OFFSET])(ed);
        else if (ed->ch == '\n' && ed->cl_a)
            process_cl_operation(ed);
        else if (ed->ch <= UCHAR_MAX
            && (isprint(ed->ch) || ed->ch == '\t' || ed->ch == '\n')
            && gb_insert_ch(a_gb, (char) ed->ch))
            debug(goto error);
    }

    return free_editor(ed);

error:
    free_editor(ed);
    debug(return 1);
}
