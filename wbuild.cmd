::
:: Copyright (c) 2025 Logan Ryan McLintock. All rights reserved.
::
:: Redistribution and use in source and binary forms, with or without
:: modification, are permitted provided that the following conditions
:: are met:
:: 1. Redistributions of source code must retain the above copyright
::    notice, this list of conditions and the following disclaimer.
:: 2. Redistributions in binary form must reproduce the above copyright
::    notice, this list of conditions and the following disclaimer in the
::    documentation and/or other materials provided with the distribution.
::
:: THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
:: ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
:: IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
:: ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
:: FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
:: DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
:: OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
:: HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
:: LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
:: OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
:: SUCH DAMAGE.
::

cl /c /Wall /DDEBUG *.c .\test\*.c

cl /Wall /DDEBUG test_buf.obj buf.obj int.obj /Fe.\test\test_buf.exe

cl /Wall /DDEBUG test_input.obj input.obj buf.obj int.obj ^
    /Fe.\test\test_input.exe

cl /Wall /DDEBUG test_screen.obj screen.obj int.obj ^
    /Fe.\test\test_screen.exe

cl /Wall /DDEBUG test_gap_buf.obj gap_buf.obj input.obj buf.obj int.obj ^
    /Fe.\test\test_gap_buf.exe

cl /Wall /DDEBUG suco.obj gap_buf.obj screen.obj input.obj buf.obj int.obj ^
    /Fesuco.exe

del *.obj
