#!/bin/bash

while sleep 0.1; do find src meson.build example | entr -d meson compile -C debug; done
