# Change Log

## [0.26.0]

### Introduced cppaper lua functions

- get_pages_with_config (See README.md for further documentation)

### New template functions

- getPagesWithConfig (See README.md for further documentation)

## [0.25.1]

- Added `process_as_template`(boolean) option to disable markdown files being processed as template (See README.md for further documentation)


## [0.25.0]

### Added

- Lua support added

-------

### Added

- `no_output` option
- `page.id` added to template option
- `getPathFrom` template function added
- Initial lua script support
- Starting migration to tfstring

### Changed

- Updated Docs
- Updated inja to 3.4

### Fixed

- Organized meson.build
- Code cleanup and more organized structure

## [0.17.6]

### Fixed

- Fixed Index relative Path (fixed in 86b8ae3)

## [0.17.0]

- Markdown files are also templated(you can use inja functions in it)
- Added `getPagesByTag` to template functions

## [0.17.4]

### Added

- Now a `sitemap.xml` is generated mapping all files

### Changed

- `markdown_unsafe` config is opt in

### Fixed - [Code Cleanup Week](https://github.com/Yrds/cppaper/issues/8)
- Create a special function to init function system (fixed in 33ae442)
- Opt in markdown_safe config to true. (fixed in 6936812)
- Adjust RelativePathComponent which is not showing json and md files path to .html destination. (fixed in bdf910d)

## [0.17.5] Hotfix

### Fixed

- Fixed Index pages not being included in `sitemap.xml` (fixed in 670e4deb)

## [0.17.6] Hotfix

### Fixed

- Fixed Index relative Path (fixed in 86b8ae31)
