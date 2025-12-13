#! /bin/sh

#
# Copyright (c) 2025 Logan Ryan McLintock
#
# Licensed under the Microsoft Reciprocal License (MS-RL) (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License in the LICENSE file or at
#
#     https://opensource.org/license/ms-rl-html
#

# Safe version control.

set -e
set -u
set -x

# Configuration.
backup_location=$HOME/git_backups


if [ "$#" -eq 1 ] && [ "$1" = init ]
then
    if [ -d .git ]
    then
        printf '%s: ERROR: Git repository already exists\n' "$0" 1>&2
        exit 1
    fi
else
    if [ ! -d .git ]
    then
        printf '%s: ERROR: Not in a Git repository parent directory\n' \
            "$0" 1>&2

        exit 1
    fi
fi

wd=$(pwd)
dst=$backup_location/$(date +%Y_%m_%d_%H_%M_%S)_$(basename "$wd")

mkdir -p "$backup_location"
cpdup -I "$wd" "$dst"

# Need to use the full path so that this script is not called again.
/usr/bin/git "$@"
