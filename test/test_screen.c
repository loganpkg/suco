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

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>

#include "../alias.h"
#include "../debug.h"
#include "../screen.h"

#define ALPHABET_SIZE 26


int print_alphabet(Screen sc, int mode, int increment)
{
    unsigned char u;
    size_t i, j;

    u = 'A';
    for (i = 0; i < ALPHABET_SIZE; ++i) {
        if (clear_screen(sc, mode))
            debug(return 1);

        for (j = 0; j <= i; j++)
            print_ch(sc, u);

        if (refresh_screen(sc))
            debug(return 1);

        if (increment)
            ++u;

        sleep(1);
    }
    return 0;
}


int main(void)
{
    Screen sc;

    if ((sc = init_screen()) == NULL)
        debug(return 1);

    sleep(2);

    if (print_alphabet(sc, HARD_CLEAR, 0))
        debug(goto error);

    if (print_alphabet(sc, SOFT_CLEAR, 0))
        debug(goto error);

    if (print_alphabet(sc, HARD_CLEAR, 1))
        debug(goto error);

    if (print_alphabet(sc, SOFT_CLEAR, 1))
        debug(goto error);

    highlight_on(sc);

    if (print_str(sc, "cool world\n"))
        debug(goto error);

    highlight_off(sc);

    if (print_str(sc, "\x01\x1B\n"))
        debug(goto error);

    if (move(sc, 0, 4))
        debug(goto error);

    if (refresh_screen(sc))
        debug(goto error);

    sleep(1);

    while (!print_str(sc, "\x05\xFF\telephant"))
        if (refresh_screen(sc))
            debug(goto error);

    sleep(1);

    return free_screen(sc);

  error:
    free_screen(sc);
    debug(return 1);
}
