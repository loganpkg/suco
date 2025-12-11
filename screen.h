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
