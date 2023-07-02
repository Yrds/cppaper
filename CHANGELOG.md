# Changelog (since 0.17.0)

## [0.17.0]

- Markdown files are also templated(you can use inja functions in it)
- Added `getPagesByTag` to template functions

## [0.17.4]

## Added

- Now a `sitemap.xml` is generated mapping all files

## Changed

- `markdown_unsafe` config is opt in

## Fixed - [Code Cleanup Week](https://github.com/Yrds/cppaper/issues/8)
- Create a special function to init function system (fixed in 33ae442)
- Opt in markdown_safe config to true. (fixed in 6936812)
- Adjust RelativePathComponent which is not showing json and md files path to .html destination. (fixed in bdf910d)
