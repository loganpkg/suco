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

#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif


#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/ioctl.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alias.h"
#include "debug.h"
#include "int.h"
#include "screen.h"


/* ANSI escape sequences: */
#define es_clear()        printf("\x1B[2J")
#define es_reset()        printf("\x1B[0m")
#define es_reverse_on()   printf("\x1B[7m")
#define es_reverse_off()  printf("\x1B[27m")
#define es_blinking_off() printf("\x1B[25m")
#define es_hide_cursor()  printf("\x1B[25l")
#define es_show_cursor()  printf("\x1B[25h")

/* y and x start from 0. */
#define es_move(y, x)     printf("\x1B[%lu;%luH", \
    (size_t) (y) + 1, (size_t) (x) + 1)


struct screen {
    int fd;
    size_t h;                   /* Screen height. */
    size_t w;                   /* Screen width. */
    size_t area;                /* Screen area. */
    size_t y;                   /* Vertical coordinate (starts at 0). */
    size_t x;                   /* Horizontal coordinate (starts at 0). */
#ifdef _WIN32
    HANDLE console_handle;
    int mode_backup;            /* Indicates if mode_orig has been saved. */
    /* Used to restore the original console settings. */
    DWORD mode_orig;
#endif
    /* Double buffering: */
    unsigned char *current_mem; /* Mirrors the displayed screen. */
    unsigned char *next_mem;    /* Used to prepare for the next display. */
};


int clear_screen(Screen sc, int mode)
{
    size_t h, w, area;
    unsigned char *t;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO console_info;
#else
    struct winsize ws;
#endif

    if (mode != HARD_CLEAR && mode != SOFT_CLEAR)
        debug(return 1);

#ifdef _WIN32
    if (!GetConsoleScreenBufferInfo(sc->console_handle, &console_info))
        debug(return 1);

    h = console_info.srWindow.Bottom - console_info.srWindow.Top + 1;
    w = console_info.srWindow.Right - console_info.srWindow.Left + 1;
#else
    if (ioctl(sc->fd, TIOCGWINSZ, &ws))
        debug(return 1);

    h = ws.ws_row;
    w = ws.ws_col;
#endif

    if (mult_overflow(h, w))
        debug(return 1);

    area = h * w;

    if (area > sc->area) {
        /*
         * Since the memory only gets increased, it is OK if one realloc
         * succeeds and one fails.
         */
        if ((t = realloc(sc->current_mem, area)) == NULL)
            debug(return 1);

        sc->current_mem = t;

        /* Only clear the extended memory. */
        if (mode != HARD_CLEAR)
            memset(sc->current_mem + sc->area, ' ', area - sc->area);

        if ((t = realloc(sc->next_mem, area)) == NULL)
            debug(return 1);

        sc->next_mem = t;
    }

    sc->h = h;
    sc->w = w;
    sc->area = area;

    if (mode == HARD_CLEAR) {
        es_clear();
        es_reset();
        es_blinking_off();
        es_hide_cursor();

        memset(sc->current_mem, ' ', sc->area);
    }

    memset(sc->next_mem, ' ', sc->area);
    sc->y = 0;
    sc->x = 0;

    return 0;
}


int free_screen(Screen sc)
{
    int r = 0;

    if (sc != NULL) {
#ifdef _WIN32
        if (sc->mode_backup
            && !SetConsoleMode(sc->console_handle, sc->mode_orig))
            debug(r = 1);
#endif
        free(sc->current_mem);
        free(sc->next_mem);
        free(sc);
    }

    return r;
}


Screen init_screen(void)
{
#ifdef _WIN32
    HANDLE console_handle;
    DWORD mode;
#endif
    Screen sc = NULL;

    if ((sc = calloc(1, sizeof(struct screen))) == NULL)
        debug(goto error);

    /* Do not assume NULL is zero. */
    sc->current_mem = NULL;
    sc->next_mem = NULL;

    if ((sc->fd = fileno(stdout)) == -1)
        debug(goto error);

#ifdef _WIN32
    sc->console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (sc->console_handle == INVALID_HANDLE_VALUE
        || sc->console_handle == NULL)
        debug(goto error);

    if (!GetConsoleMode(sc->console_handle, &mode))
        debug(goto error);

    sc->mode_orig = mode;
    sc->mode_backup = 1;

    /* Set ANSI mode. */
    mode |=
        ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING |
        DISABLE_NEWLINE_AUTO_RETURN;
    mode &= ~(ENABLE_WRAP_AT_EOL_OUTPUT | ENABLE_LVB_GRID_WORLDWIDE);

    if (!SetConsoleMode(sc->console_handle, mode))
        debug(goto error);

#endif

    if (clear_screen(sc, HARD_CLEAR))
        debug(goto error);

    return sc;

  error:
    free_screen(sc);
    return NULL;
}


void get_screen_height(Screen sc, size_t *height)
{
    *height = sc->h;
}


void get_screen_width(Screen sc, size_t *width)
{
    *width = sc->w;
}


int print_ch(Screen sc, char ch)
{
    size_t i;

    i = sc->y * sc->w + sc->x;
    if (i == sc->area)
        debug(return 1);        /* Off screen. */

    sc->next_mem[i] = ch;

    ++sc->x;
    if (sc->x == sc->w) {
        /* Line wrap. */
        ++sc->y;
        sc->x = 0;
    }
    return 0;
}


int refresh_screen(Screen sc)
{
    size_t y, x;                /* In memory. */
    size_t s_y, s_x;            /* On displayed screen. */
    unsigned char u;
    unsigned char *t;

    es_hide_cursor();
    for (y = 0; y < sc->h; ++y)
        for (x = 0; x < sc->w; ++x)
            if ((u =
                 sc->next_mem[y * sc->w + x]) !=
                sc->current_mem[y * sc->w + x]) {

                /* Optimisation to avoid unneeded moves. */
                if (y != s_y || x != s_x) {
                    es_move(y, x);
                    s_y = y;
                    s_x = x;
                }

                putchar(u);     /* Advances the cursor on the display. */
                /* Track the displayed cursor's location. */
                ++s_x;
                if (s_x == sc->w) {
                    /* Wrap. */
                    ++s_y;
                    s_x = 0;
                }
            }

    /* Set the final displayed cursor location. */
    if (sc->y != s_y || sc->x != s_x)
        es_move(sc->y, sc->x);

    es_show_cursor();
    if (fflush(stdout))
        return 1;

    /* Switch memory. */
    t = sc->current_mem;
    sc->current_mem = sc->next_mem;
    sc->next_mem = t;

    return 0;
}
