#! /bin/sh

#
# Copyright (c) 2025 Logan Ryan McLintock. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#

set -e
set -u
set -x


# Configuration.
c_ops='-ansi -g -O0 -Wall -Wextra -pedantic -I. -DDEBUG'
indent_ops='-kr -nut -l79'

export c_ops

wd=$(pwd)
export wd

build_dir=$(mktemp -d)
error_file="$build_dir"/error_file
export error_file


typedefs=$(grep -r --exclude-dir='.git' -E typedef \
    | grep -E -o '[a-zA-Z_]+;$' \
    | sed -E 's/(.*);/-T \1/' \
    | tr '\n' ' ' \
    | sed -E 's/ +$//')

indent_ops="$indent_ops $typedefs"
export indent_ops


build_c() {
    # shellcheck disable=SC2086
    find . -type f \( -name '*.h' -o -name '*.c' \) -exec sh -c '
        set -e
        set -u
        set -x
        fn="$1"
        # shellcheck disable=SC2086
        if ! cc -c $c_ops "$fn"
        then
            printf "%s: ERROR: cc failed: %s\n" "$0" "$fn" \
                >> "$error_file"
        fi
    ' sh '{}' \;
}


exit_if_error() {
    if [ -s "$error_file" ]
    then
        printf "%s: Error file:\n" "$0" 1>&2
        cat "$error_file" 1>&2
        exit 1
    fi
}


fix_perms() {
    find . -type f ! -path '*.git/*' -exec chmod 600 '{}' \;
    find . -type d ! -path '*.git/*' -exec chmod 700 '{}' \;

    find . -type f ! -path '*.git/*' \
        \( -name '*.sh' -o -name '*.exp' -o -name '*.cmd' \) \
        -exec chmod 700 '{}' \;
}


fix_perms


# Copy files to build directory.
rsync -av --exclude '.git/' . "$build_dir"

cd "$build_dir" || exit 1

find . -type f -name '*.sh' -exec sh -c \
    'fn="$1"; shellcheck "$1" >> "$error_file"' 'sh' '{}' \;

exit_if_error

# Update header files.
find . -type f -name '*.h' -exec sh -c '
        set -e
        set -u
        set -x
        fn="$1"
        c_fn=$(printf %s "$fn" | sed -E "s/\.h$/.c/")
        if [ ! -s "$c_fn" ]
        then
            exit 0
        fi
        line_num=$(grep -n -E "Function declarations" "$fn" | cut -d ":" -f 1)
        if [ -z "$line_num" ]
        then
           printf "%s: ERROR: Function declarations not in header: %s\n" \
                "$0" "$fn" >> "$error_file"

           exit 1
        fi
        head -n "$line_num" "$fn" > "$fn"~
        # shellcheck disable=SC2086
        if ! indent $indent_ops "$c_fn"
        then
            printf "%s: ERROR: indent failed: %s\n" "$0" "$c_fn" \
                >> "$error_file"
        fi
        {
            grep -E -v -e "^(static |typedef )" -e ":" "$c_fn" \
                | tr -d "~" | tr "\n" "~" \
                | grep -E -o "~[a-zA-Z_][^}{]+\)~" | tr -d "\n" \
                | sed -E "s/^~//" | tr "~" "\n" | sed -E "s/\)$/);/"

            printf "\n%s\n" "#endif"
        } >> "$fn"~
        mv "$fn"~ "$fn"
    ' sh '{}' \;

exit_if_error

build_c

exit_if_error

# shellcheck disable=SC2086
find . -type f \( -name '*.h' -o -name '*.c' \) -exec sh -c '
        set -e
        set -u
        set -x
        fn="$1"
        # shellcheck disable=SC2086
        if ! indent $indent_ops "$fn"
        then
            printf "%s: ERROR: indent failed: %s\n" "$0" "$fn" \
                >> "$error_file"
        fi
    ' sh '{}' \;

exit_if_error

# Clean up.
find . -type f -name '*~' -delete

# Check for long lines.
if grep -r -I --exclude-dir='.git' -E '.{80}' >> "$error_file"
then
    printf '%s: ERROR: Long lines.\n' "$0" >> "$error_file"
    exit 1
fi

exit_if_error

build_c

exit_if_error

# shellcheck disable=SC2086
cc $c_ops test_buf.o buf.o int.o -o test/test_buf
# shellcheck disable=SC2086
cc $c_ops test_input.o input.o buf.o int.o -o test/test_input
# shellcheck disable=SC2086
cc $c_ops test_screen.o screen.o int.o -o test/test_screen
# shellcheck disable=SC2086
cc $c_ops test_gap_buf.o gap_buf.o screen.o input.o buf.o int.o \
    -o test/test_gap_buf

# shellcheck disable=SC2086
cc $c_ops test_dll.o doubly_linked_list.o \
    -o test/test_dll

# shellcheck disable=SC2086
cc $c_ops suco.o gap_buf.o screen.o input.o buf.o int.o -o suco


# Move source code back.
find . -type f \( -name '*.h' -o -name '*.c' \) -exec sh -c '
        set -e
        set -u
        set -x
        fn="$1"
        mv "$fn" "$wd/$fn"
    ' sh '{}' \;


cd "$wd" || exit 1
fix_perms
cd "$build_dir" || exit 1


# Move executables.
valgrind ./test/test_buf
# valgrind ./test/test_input
mv test/test_buf "$wd"/test/test_buf
mv test/test_input "$wd"/test/test_input
mv test/test_screen "$wd"/test/test_screen
mv test/test_gap_buf "$wd"/test/test_gap_buf
mv test/test_dll "$wd"/test/test_dll
mv suco "$wd"/suco

cp safe_vc.sh "$HOME"/bin/safe_vc
