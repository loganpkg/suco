#! /bin/sh

#
# Copyright (c) 2025 Logan Ryan McLintock. All rights reserved.
#
# The contents of this file are subject to the
# Common Development and Distribution License (CDDL) version 1.1
# (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License in
# the LICENSE file included with this software or at
# https://opensource.org/license/cddl-1-1
#
# NOTICE PURSUANT TO SECTION 4.2 OF THE LICENSE:
# This software is prohibited from being distributed or otherwise made
# available under any subsequent version of the License.
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
# License for the specific language governing rights and limitations
# under the License.
#

set -e
set -u
set -x

c_ops='-ansi -g -O0 -Wall -Wextra -pedantic'
indent_ops='-kr -nut -l80 -T Buf'


build_c() {
    # shellcheck disable=SC2086
    find . -type f -name '*.h' -exec cc -c $c_ops '{}' \;
    # shellcheck disable=SC2086
    find . -type f -name '*.c' -exec cc -c $c_ops '{}' \;
}


wd=$(pwd)
export wd
build_dir=$(mktemp -d)
error_file="$build_dir"/error_file
export error_file


# Copy files to build directory.
rsync -av --exclude '.git/' . "$build_dir"

cd "$build_dir" || exit 1

find . -type f -name '*.sh' -exec sh -c \
    'fn="$1"; shellcheck "$1" >> "$error_file"' 'sh' '{}' \;

# Update header files.
find . -type f -name '*.h' -exec sh -c '
        set -e
        set -u
        set -x
        fn="$1"
        line_num=$(grep -n -E "Function declarations" "$fn" | cut -d ":" -f 1)
        if [ -z "$line_num" ]
        then
           printf "%s: ERROR: Function declarations not in header: %s\n" \
                "$0" "$fn" >> "$error_file"

           exit 1
        fi
        head -n "$line_num" "$fn" > "$fn"~
        c_fn=$(printf %s "$fn" | sed -E "s/\.h$/.c/")
        {
            grep -E "^[a-zA-Z][^}{]+$" "$c_fn" | sed -E "s/$/;/"
            printf "\n%s\n" "#endif"
        } >> "$fn"~
        mv "$fn"~ "$fn"
    ' sh '{}' \;

if [ -s "$error_file" ]
then
    cat "$error_file"
    exit 1
fi

build_c

# shellcheck disable=SC2086
find . -type f \( -name '*.h' -o -name '*.c' \) \
    -exec indent $indent_ops '{}' \;

# Check for long lines.
if grep -r -I --exclude-dir='.git' -E '.{80}'
then
    printf '%s: ERROR: Long lines.\n' "$0"
    exit 1
fi

build_c

# Move C files back.
find . -type f \( -name '*.h' -o -name '*.c' \) -exec sh -c '
        set -e
        set -u
        set -x
        fn=$(basename "$1")
        mv "$fn" "$wd/$fn"
    ' sh '{}' \;

# shellcheck disable=SC2086
cc $c_ops test_buf.o buf.o int.o -o test_buf
valgrind ./test_buf
mv test_buf "$wd"/test_buf
