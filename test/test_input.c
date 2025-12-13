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

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "../alias.h"
#include "../input.h"


int main(void)
{
    Input ip;
    int r, ch;
    int non_blocking = 0;

    struct key_map km[] = {
        { { CTRL_A, CTRL_B, CTRL_C, CTRL_D, CTRL_E, CTRL_F}, 0x300 },
        { { ESC, 'x'}, 0x301 },
        { { KEY_F1, KEY_F2, KEY_F12, KEY_DELETE, KEY_PAGE_UP}, 0x302 },
        { { 0}, 0 }
    };

    if ((ip = init_input_stdin(BLOCKING, RAW, NULL)) == NULL)
        return 1;

    while (1) {
        printf("=> ");

        if (fflush(stdout)) {
            free_input(ip);
            return 1;
        }

        if (non_blocking)
            sleep(2);

        r = get_ch(ip, &ch);
        if (r == 1) {
            free_input(ip);
            return 1;
        }

        if (ch == EOF)
            break;

        if (r == WOULD_BLOCK)
            printf("_ \r\n");
        else
            printf("%02X \r\n", ch);

        if (fflush(stdout)) {
            free_input(ip);
            return 1;
        }

        if (ch == 'q')
            break;
        else if (ch == 'n') {
            if (free_input(ip))
                return 1;

            if ((ip = init_input_stdin(NON_BLOCKING_TTY, COOKED, NULL))
                == NULL)
                return 1;

            non_blocking = 1;
        } else if (ch == 'd') {
            if (free_input(ip))
                return 1;

            if ((ip = init_input_stdin(NON_BLOCKING_TTY, DOUBLE_COOKED, km))
                == NULL)
                return 1;

            non_blocking = 1;
        }
    }

    if (free_input(ip))
        return 1;

    return 0;
}
