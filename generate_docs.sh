#!/bin/bash
#

if [[ -z ${MESON_BUILD_ROOT} ]]; then
  echo "This script must be runned by 'meson compile -C build docs' command"
else
  cp $MESON_SOURCE_ROOT/README.md $MESON_SOURCE_ROOT/docs_src/pages/index.md
  "$MESON_BUILD_ROOT/cppaper" -C "$MESON_SOURCE_ROOT/docs_src" -O "$MESON_SOURCE_ROOT/public"
fi
