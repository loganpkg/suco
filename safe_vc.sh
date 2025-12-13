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
