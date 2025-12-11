/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <stddef.h>

#define HARD_CLEAR 1
#define SOFT_CLEAR 2

typedef struct screen *Screen;

extern int dummy;


/* Function declarations */
int clear_screen(Screen sc, int mode);

int free_screen(Screen sc);

Screen init_screen(void);

void get_screen_height(Screen sc, size_t *height);

void get_screen_width(Screen sc, size_t *width);

int print_ch(Screen sc, char ch);

int refresh_screen(Screen sc);

#endif
