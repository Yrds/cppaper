# CPPaper

A static blog/site generator made in modern c++.

## Features

  - Templates using [inja](https://github.com/pantor/inja)
  - Pages using markdown(cfg version]

## Installing

### Package management

Not ready yet!

### Building

#### Requirements

- meson
- Any compiler which supports c++20

#### Instructions

```
meson setup build
meson compile -C build
```

After the build, a binary will be available at `build/cppaper`

If you want to install alongside with needed dependencies it you must enter this command:

`meson install -C build --destdir /opt/cppaper`

Where `--destdir` will be the installation directory.

Note: In case you don't have permission to install to a specific directory, you can use `sudo` with `meson install` safely.

## Using/Example

I want to provide a more detailed guide on how to use `cppaper`, but for now I have a "example" directory with a fully working site, just run `cppaper` inside this directory and everything should work.
