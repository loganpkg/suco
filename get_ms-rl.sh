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

# Coverts the MS-RL from HTML to text.

set -e
set -u
set -x

curl -sL https://opensource.org/license/ms-rl-html \
    | pup 'h1[class="entry-title page--title"],'\
        'span[class="license-spdx"],'\
        'div[class="entry-content post--content license-content"]'\
        'text{}' \
    | sed -E -e 's/\xE2\x80(\x9C|\x9D)/"/g' \
        -e "s/\xE2\x80\x99/'/g" \
        -e 's/\t/    /g' \
        -e 's/^ +//' \
        -e 's/ +$//' \
    | tr '\n' '\0' \
    | sed -E -e 's/\x00\x00\x00+/\x00\x00/g' \
        -e 's/\x00\x00SPDX short identifier:\x00([A-Z-]*)\x00/ (\1)\x00/g' \
        -e 's/\x00\x00+$/\x00/' \
    | tr '\0' '\n' \
    | fold -w 80 -s \
    | sed -E 's/ +$//' \
    > LICENSE

if grep -E '.{80}' LICENSE
then
    printf '%s: ERROR: Lines too long\n' "$0"
    exit 1
fi
