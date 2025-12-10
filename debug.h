/*
 * Copyright (c) 2025 Logan Ryan McLintock
 *
 * Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License in the LICENSE file or at
 *
 *     https://opensource.org/license/ms-rl-html
 */

#ifndef DEBUG_H
#define DEBUG_H

/* Define on the command line. */
#ifdef DEBUG

#define debug(code) do { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #code); \
    code; \
} while (0)

#else

#define debug(code) do { \
    code; \
} while (0)

#endif

extern int dummy;

/* Function declarations */

#endif
