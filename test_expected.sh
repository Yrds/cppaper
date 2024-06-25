#!/usr/bin/env bash

set -x

$MESON_BUILD_ROOT/cppaper -C $MESON_SOURCE_ROOT/example -O $MESON_SOURCE_ROOT/example/public

find $MESON_SOURCE_ROOT/example/expected -type f | xargs -I '{}' bash -c 'export file="{}";diff --color $file ${file/expected/public}'
