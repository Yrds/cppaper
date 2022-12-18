#!/bin/bash
#

if [[ -z ${MESON_BUILD_ROOT} ]]; then
  echo "This script must be runned by 'meson compile -C build docs' command"
else
  "$MESON_BUILD_ROOT/cppaper" -C "$MESON_SOURCE_ROOT/docs_src" -O "$MESON_SOURCE_ROOT/docs"
fi
