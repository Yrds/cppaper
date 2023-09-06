#!/bin/bash

while sleep 0.1; do find docs_src/pages | entr -d meson compile -C build docs; done
