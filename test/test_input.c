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
