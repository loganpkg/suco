/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#ifndef ALIAS_H
#define ALIAS_H


#ifdef _WIN32
#define fileno _fileno
#define isatty _isatty
#define sleep(sec) Sleep((sec) * 1000)
#endif

extern int dummy;

/* Function declarations */

#endif
