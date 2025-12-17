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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buf.h"
#include "debug.h"
#include "gap_buf.h"
#include "input.h"
#include "int.h"


/* Operation type: */
#define INSERT 1
#define DELETE 2
#define BEGIN_MULTI 4
#define END_MULTI 8


/* Mode: */
#define NORMAL 1
#define UNDO 2
#define REDO 4


/*
 * When undoing, the undo buffer is used to replay (the opposite)
 * of operations. But when redoing, the redo buffer is used to replay.
 */
#define replay_buf(gb) ((gb)->mode == UNDO ? (gb)->undo : (gb)->redo)

/*
 * Normally operations are recorded in the undo buffer,
 * including when redoing.
 * But, when undoing, the operations are recorded in the redo buffer.
 */
#define record_buf(gb) ((gb)->mode == UNDO ? (gb)->redo : (gb)->undo)


struct operation {
    /* Copy of the g location. g does not change with realloc. */
    size_t g;
    unsigned char type;         /* Type of operation. */
    char ch;                    /* The inserted or deleted character. */
};


    /*
     * The region is the text between the mark (inclusive) and the
     * cursor (exclusive), or the cursor (inclusive) and the
     * mark (exclusive), depending on which one comes first.
     */

struct gap_buf {
    char *fn;                   /* Filename associated with the gap buffer. */
    Buf undo;                   /* Undo stack. */
    Buf redo;                   /* Redo stack. */
    int mode;                   /* Mode: NORMAL, UNDO, REDO. */
    char *a;                    /* Memory. */
    size_t g;                   /* Start of gap. */
    size_t c;                   /* Cursor. */
    size_t e;                   /* End of gap buffer (included in memory). */
    size_t m;                   /* Mark. This is a saved "g-value." */
    int m_set;                  /* Indicates that the mark is set. */
    size_t row;                 /* Cursor row. Starts from 1. */
    size_t col;                 /* Cursor column. Starts from 0. */
    size_t d;                   /* Draw start. This can be compared with g. */
    int rc;                     /* Request centring. */
    size_t mod;                 /* Modified indicator. */
};


void free_gap_buf(Gap_buf gb)
{
    if (gb != NULL) {
        free(gb->fn);
        free_buf(gb->undo);
        free_buf(gb->redo);
        free(gb->a);
        free(gb);
    }
}


Gap_buf init_gap_buf(size_t init_num_elements)
{
    Gap_buf gb = NULL;

    if (!init_num_elements)
        debug(goto error);

    if ((gb = calloc(1, sizeof(struct gap_buf))) == NULL)
        debug(goto error);

    /* Do not assume that NULL is zero. */
    gb->fn = NULL;
    gb->undo = NULL;
    gb->redo = NULL;
    gb->a = NULL;

    if ((gb->undo =
         init_buf(init_num_elements, sizeof(struct operation))) == NULL)
        debug(goto error);

    if ((gb->redo =
         init_buf(init_num_elements, sizeof(struct operation))) == NULL)
        debug(goto error);

    gb->mode = NORMAL;

    if ((gb->a = calloc(init_num_elements, sizeof(char))) == NULL)
        debug(goto error);

    gb->c = init_num_elements - 1;
    gb->e = init_num_elements - 1;
    *(gb->a + gb->e) = '~';     /* The end of buffer character. */
    gb->row = 1;

    return gb;

  error:
    free_gap_buf(gb);
    debug(return NULL);
}


int gb_insert_ch(Gap_buf gb, char ch)
{
    /*
     * The character is written at g, then g is incremented.
     *
     * Before:
     * woXXXXld~
     * ^ ^   ^ ^
     * | |   | |
     * a g   c e
     *
     * After:
     * worXXXld~
     * ^  ^  ^ ^
     * |  |  | |
     * a  g  c e
     *
     */

    size_t s;
    char *t;
    struct operation op;

    if (gb->g == gb->c) {
        /* Need to grow the gap. */
        s = gb->e + 1;          /* Cannot overflow, as already in memory. */
        if (mult_overflow(s, 2))
            debug(return 1);

        if ((t = realloc(gb->a, s * 2)) == NULL)
            debug(return 1);

        gb->a = t;

        /* Move down data after the gap. */
        memmove(gb->a + gb->c + s, gb->a + gb->c, gb->e - gb->c + 1);

        /* Update values that are after the gap. */
        gb->c += s;
        gb->e += s;
    }

    /* Record the operation. */
    op.g = gb->g;               /* Record g before it changes. */
    op.type = INSERT;
    op.ch = ch;

    if (push(record_buf(gb), &op))
        debug(return 1);

    /* Need to truncate the redo buffer when in normal mode. */
    if (gb->mode == NORMAL)
        truncate_buf(gb->redo);

    /* Write character to the left of the gap, making the gap smaller. */
    *(gb->a + gb->g++) = ch;

    /* Update row number and column number. */
    if (ch == '\n') {
        ++gb->row;
        gb->col = 0;
    } else {
        ++gb->col;
    }

    /* Clear the mark. */
    gb->m_set = 0;
    gb->m = 0;

    /* Set the modified indicator. */
    gb->mod = 1;

    return 0;
}


int gb_delete_ch(Gap_buf gb)
{
    /*
     * c is incremented.
     *
     * Before:
     * elepXXXXjhant~
     * ^   ^   ^    ^
     * |   |   |    |
     * a   g   c    e
     *
     * After:
     * elepXXXXXhant~
     * ^   ^    ^   ^
     * |   |    |   |
     * a   g    c   e
     *
     * Not displaying the gap:
     * Before:
     * elepjhant~
     *     ^
     *     |
     *     c
     *
     * After:
     * elephant~
     *     ^
     *     |
     *     c
     *
     */

    struct operation op;

    /* Cannot delete the last character in the gap buffer. */
    if (gb->c == gb->e)
        return 1;

    op.g = gb->g;
    op.type = DELETE;
    op.ch = *(gb->a + gb->c);

    /* Record the operation. */
    if (push(record_buf(gb), &op))
        debug(return 1);

    /* Need to truncate the redo buffer when in normal mode. */
    if (gb->mode == NORMAL)
        truncate_buf(gb->redo);

    /* Expand the gap to the right. */
    ++gb->c;

    gb->m_set = 0;
    gb->m = 0;

    gb->mod = 1;

    return 0;
}


int gb_left_ch(Gap_buf gb)
{
    /*
     * g and c are decremented, then the character at g is copied to c.
     *
     * Before:
     * worXXXXld~
     * ^  ^   ^ ^
     * |  |   | |
     * a  g   c e
     *
     * Decrement:
     * worXXXXld~
     * ^ ^   ^  ^
     * | |   |  |
     * a g   c  e
     *
     * After:
     * woXXXXrld~
     * ^ ^   ^  ^
     * | |   |  |
     * a g   c  e
     *
     */

    size_t i;

    if (!gb->g)
        return 1;               /* At start of gap buffer. */

    if ((*(gb->a + --gb->c) = *(gb->a + --gb->g)) == '\n') {
        --gb->row;              /* Gone up a line. */

        /* Need to recalculate the column number. */
        i = gb->g;
        while (i && *(gb->a + i--) != '\n')
            ++gb->col;
    } else {
        --gb->col;
    }

    return 0;
}


int gb_right_ch(Gap_buf gb)
{
    /*
     * The character at c is copied to g, then g and c are incremented.
     *
     * Before:
     * woXXXXrld~
     * ^ ^   ^  ^
     * | |   |  |
     * a g   c  e
     *
     * Copy:
     * worXXXrld~
     * ^ ^   ^  ^
     * | |   |  |
     * a g   c  e
     *
     * After:
     * worXXXXld~
     * ^  ^   ^ ^
     * |  |   | |
     * a  g   c e
     *
     */

    if (gb->c == gb->e)
        return 1;               /* At end of the buffer. */

    if ((*(gb->a + gb->g++) = *(gb->a + gb->c++)) == '\n') {
        ++gb->row;
        gb->col = 0;
    } else {
        ++gb->col;
    }

    return 0;
}


static int record_multi(Gap_buf gb, unsigned char type)
{
    struct operation op;

    if (type != BEGIN_MULTI && type != END_MULTI)
        debug(return 1);

    op.g = 0;
    op.type = type;
    op.ch = '\0';

    if (push(record_buf(gb), &op))
        debug(return 1);

    return 0;
}


static int undo(Gap_buf gb, int mode)
{
    struct operation op;
    size_t depth;

    if (mode != UNDO && mode != REDO)
        debug(goto error);

    gb->mode = mode;

    depth = 0;
    do {
        if (pop(replay_buf(gb), &op))
            break;              /* No more. */

        if (op.type == BEGIN_MULTI || op.type == END_MULTI) {
            if (op.g || op.ch)  /* These should not be used. */
                debug(goto error);
        } else {
            /* Move into position. */
            while (gb->g < op.g)
                if (gb_right_ch(gb))
                    debug(goto error);  /* Should not fail. */

            while (gb->g > op.g)
                if (gb_left_ch(gb))
                    debug(goto error);  /* Should not fail. */

            /* Check. */
            if (gb->g != op.g)
                debug(goto error);      /* Should not fail. */
        }

        /* Perform the opposite operation. */
        switch (op.type) {
        case INSERT:
            if (gb_delete_ch(gb))
                debug(goto error);

            break;
        case DELETE:
            if (gb_insert_ch(gb, op.ch))
                debug(goto error);

            break;
        case BEGIN_MULTI:
            --depth;
            if (record_multi(gb, END_MULTI))
                debug(goto error);

            break;
        case END_MULTI:
            ++depth;
            if (record_multi(gb, BEGIN_MULTI))
                debug(goto error);

            break;
        default:
            debug(goto error);  /* Invalid operation type. */
        }
    } while (depth);

    gb->mode = NORMAL;
    return 0;

  error:
    gb->mode = NORMAL;
    debug(return 1);
}


int gb_undo(Gap_buf gb)
{
    return undo(gb, UNDO);
}


int gb_redo(Gap_buf gb)
{
    return undo(gb, REDO);
}


void gb_debug_print(Gap_buf gb)
{
    /* Useful for debugging. */
    size_t i;

    /* Before gap. */
    for (i = 0; i < gb->g; ++i)
        putchar(*(gb->a + i));

    /* In gap. */
    for (i = gb->g; i < gb->c; ++i)
        putchar('X');

    /* After gap. */
    for (i = gb->c; i <= gb->e; ++i)
        putchar(*(gb->a + i));

    putchar('\n');
}


int gb_insert_file(Gap_buf gb, const char *fn)
{
    Input ip = NULL;
    int ch;

    if ((ip = init_input_fn(fn, BLOCKING, RAW, NULL)) == NULL)
        debug(goto error);

    if (record_multi(gb, BEGIN_MULTI))
        debug(goto error);

    while (1) {
        if (get_ch(ip, &ch))
            debug(goto error);

        if (ch == EOF)
            break;

        if (gb_insert_ch(gb, ch))
            debug(goto error);
    }

    if (record_multi(gb, END_MULTI))
        debug(goto error);

    return free_input(ip);

  error:
    free_input(ip);
    return 1;
}


int gb_set_fn(Gap_buf gb, const char *fn)
{
    size_t len;
    char *t;

    if (fn == NULL) {
        if (gb->fn != NULL)
            free(gb->fn);

        gb->fn = NULL;
        return 0;
    }

    len = strlen(fn);

    /* Cannot overflow as already in memory. */
    if ((t = calloc(len + 1, sizeof(char))) == NULL)
        debug(return 1);

    memmove(t, fn, len + 1);
    gb->fn = t;

    return 0;
}


void gb_start_of_line(Gap_buf gb)
{
    while (gb->g && *(gb->a + gb->g - 1) != '\n')
        if (gb_left_ch(gb))
            break;
}


void gb_end_of_line(Gap_buf gb)
{
    while (*(gb->a + gb->c) != '\n')
        if (gb_right_ch(gb))
            break;
}


void gb_set_mark(Gap_buf gb)
{
    gb->m = gb->g;
    gb->m_set = 1;
}


void gb_request_centring(Gap_buf gb)
{
    gb->rc = 1;
}


#define check() do {                                            \
    ++x;                                                        \
    if (ch == '\n' || x == sub_w ) {                            \
        ++y;                                                    \
        x = 0;                                                  \
    }                                                           \
    if (!d_centre_set && y == sub_h / 2 + 1) {                  \
        /*                                                      \
         * Save d_centre for later.                             \
         * Plus 1 to move back in bounds.                       \
         * Prevents mult-position chars from being half-off.    \
         */                                                     \
        d_centre = i + 1;                                       \
        d_centre_set = 1;                                       \
        if (gb->rc) /* Stop early. */                           \
             goto end;                                          \
    }                                                           \
    if (y == sub_h) {                                           \
        /* Off sub-screen. */                                   \
        ++i;                                                    \
        goto end;                                               \
    }                                                           \
} while (0)


static int gb_centre(Gap_buf gb, size_t sub_h, size_t sub_w)
{
    /*
     * This function simulates printing backwards to see if the cursor would
     * be printed within the bounds of an abstract sub_h by sub_w screen size.
     * Draw start, d, will be changed if the cursor would be off the screen,
     * or if centring was requested (rc was set).
     */
    size_t y, x, i, j;
    char ch;
    int d_centre_set = 0;
    size_t d_centre;

    if (!sub_h || !sub_w)
        debug(return 1);

    if (mult_overflow(sub_h, sub_w))
        debug(return 1);

    if (!gb->g) {
        /* At the start of the buffer. */
        gb->d = 0;
        gb->rc = 0;
        return 0;
    }

    /*
     * Obvious off screen checks.
     * If g is less than d, then the cursor is off the screen above.
     * Every character takes at least one screen position to be printed,
     * so the area check is a good short-circuit for when the cursor is
     * far off the screen below.
     */
    if (gb->g < gb->d || gb->g - gb->d > sub_h * sub_w)
        gb->rc = 1;

    i = gb->g - 1;
    y = 0;
    x = 0;
    while (1) {
        ch = *(gb->a + i);
        if (ch == '\n') {
            check();
        } else if (ch == '\t') {
            /*
             * A long character, like a tab, can wrap multiple times when
             * the screen is narrow. So need to check after each char.
             */
            for (j = 0; j < TAB_SIZE; ++j)
                check();
        } else if (iscntrl(ch)) {
            for (j = 0; j < CTRL_CH_SIZE; ++j)
                check();
        } else {
            check();
        }

        if (!i)
            break;

        --i;
    }

  end:

    if (gb->rc || gb->d < i) {
        /* Would be off screen, so centre. */
        if (d_centre_set)
            gb->d = d_centre;
        else
            gb->d = i;          /* Not enough text to reach the centre. */

        gb->rc = 0;
        return 0;
    }

    /* Current draw start is OK. */
    return 0;
}

#undef check


int gb_print(Gap_buf gb, Screen sc, size_t y_origin, size_t x_origin,
             size_t sub_h, size_t sub_w, int move_cursor)
{
    size_t h, w, i, y, x;

    h = get_screen_height(sc);
    w = get_screen_width(sc);

    if (add_overflow(y_origin, sub_h) || add_overflow(x_origin, sub_w))
        debug(return 1);

    if (y_origin + sub_h > h || x_origin + sub_w > w)
        debug(return 1);

    if (gb_centre(gb, sub_h, sub_w))
        debug(return 1);

    /* Place cursor at the left-hand side of the starting row. */
    if (move(sc, y_origin, x_origin))
        debug(return 1);

    /* Print before the gap. */
    i = gb->d;
    if (gb->m_set && gb->m < gb->g) {
        /* Before the region. */
        for (; i < gb->m; ++i)
            if (sub_screen_print_ch(sc, y_origin, x_origin, sub_h, sub_w,
                                    *(gb->a + i)))
                debug(return 1);

        highlight_on(sc);
    }

    for (; i < gb->g; ++i)
        if (sub_screen_print_ch(sc, y_origin, x_origin, sub_h, sub_w,
                                *(gb->a + i)))
            debug(return 1);

    if (gb->m_set && gb->m < gb->g)
        highlight_off(sc);

    /* Record location on the screen where the cursor should be. */
    y = get_y(sc);
    x = get_x(sc);

    /* Cursor out of designated sub-screen area. */
    if (y >= y_origin + sub_h || x >= x_origin + sub_w)
        debug(return 1);

    /* Print after the gap. */

    i = gb->c;
    /*
     * Print the cursor, always without highlighting.
     * Failure is OK, as only the first char of a multi-byte character,
     * or a newline char which is printed as a sequence of space chars
     * until the edge of the screen, needs to be on the screen.
     */
    sub_screen_print_ch(sc, y_origin, x_origin, sub_h, sub_w, *(gb->a + i));

    ++i;

    /* Region. */
    if (gb->m_set && gb->m > gb->g) {
        highlight_on(sc);
        /*
         * m must be compared to g.
         * Failure is OK, as cursor has been printed.
         */
        for (; i < gb->c + (gb->m - gb->g); ++i)
            sub_screen_print_ch(sc, y_origin, x_origin, sub_h, sub_w,
                                *(gb->a + i));

        highlight_off(sc);
    }

    /* Failure is OK. */
    for (; i <= gb->e; ++i)
        sub_screen_print_ch(sc, y_origin, x_origin, sub_h, sub_w,
                            *(gb->a + i));

    /* Place cursor in the correct location. */
    if (move_cursor && move(sc, y, x))
        debug(return 1);

    return 0;
}
