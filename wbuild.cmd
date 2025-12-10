::
:: Copyright (c) 2025 Logan Ryan McLintock
::
:: Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
:: you may not use this file except in compliance with the License.
:: You may obtain a copy of the License in the LICENSE file or at
::
::     https://opensource.org/license/ms-rl-html
::

cl /c /Wall /DDEBUG *.c .\test\*.c

cl /Wall /DDEBUG test_buf.obj buf.obj int.obj /Fe.\test\test_buf.exe

cl /Wall /DDEBUG test_input.obj input.obj buf.obj int.obj ^
    /Fe.\test\test_input.exe

del *.obj
