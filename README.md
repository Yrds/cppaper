# CPPaper

A static blog/site generator made in modern c++.

## Features

  - Templates using [inja](https://github.com/pantor/inja)
  - Pages using markdown(gfm with all extensions enabled)
  - Pages using json(nlohmann)
  - Custom Lua scripts(sol2)
  - Sitemap.xml generation

## Installing

### Package management

Aur:

`yay -S cppaper`

NOTE: Aur is temporary disabled

### Building

#### Requirements

- meson
- Any compiler which supports c++20

All libraries are downloaded automatically

#### Instructions

```
meson setup build
meson compile -C build
./build/cppaper -C example # [Optional] Run cppaper against example project to test it!
```

After the build, a binary will be available at `build/cppaper`

If you want to install alongside with needed dependencies it you must enter this command:

`meson install -C build --destdir /opt/cppaper`

Where `--destdir` will be the installation directory.

Note: In case you don't have permission to install to a specific directory, you can use `sudo` with `meson install` safely.

## Using/Example

I want to provide a more detailed guide on how to use `cppaper`, but for now I have a "example" directory with a fully working site, just run `cppaper` inside this directory and everything should work.

### Simple documentation

Some configs aren't listed yet.
Configs have the following hierarchy: `Site -> Directory -> File`.
For example, if Site has `markdown_unsafe` set to `true` then its directories and files will have the same configuration, unless you overwrite the config

Here's an example of directory structure with configs to Site, Directory and File:

```
- Project
  - config #This will set configs for all site
  - pages
    - config #This will set configs for pages directory
    - index.md 
    - index.md.config #Config only for index.md file (eg. use a different template)
```

#### Site

|Config       |Type       |Description                                                                                                                            |Example     |
|-------------|-----------|---------------------------------------------------------------------------------------------------------------------------------------|------------|
|domain       |string     |Site main domain to use to generate sitemap.xml                                                                                        |example.com |
|lua_libraries|string list|Lua libraries to load. Available: `base`, `package`, `coroutine`, `string`, `os`, `math`, `table`, `debug`, `bit32`, `io`, `ffi`, `jit`|base,package|

#### Directories & Files

|Config|Type|Description|
|------|----|-----------|
|markdown_unsafe|true(default) or false|Allow use of custom htmls in markdown|
|template|file (Ex. template/cppaper-default/default.html)|Specify a template to be used with a markdown or other content file|
|index|file|Specify a file to be used as index.html from templates directory|
|tags|comma separated strings|File tags that can by used to be queried on templates|
|no_output|false(default) or true|Exclude file from being output(also exclude from sitemap.xml and `get` like functions). Still processed as markdown and json|
|process_as_template|true(default) or false|Markdown Only - Process file content as template allowing use of inja template functions in markdown files.|


### Special directories

|Directory|Description|
|---------|-----------|
|pages|This directory goes all of your content pages: md, json, html, etc|
|templates|This is where cppaper will lookup for templates when you use `template` config|
|public|This directory can be created when you run cppaper for the first time and have all generated content from `pages` + assets directory|
|public/assets|cppaper delete every file in `public/*` except `public/assets/*`. Use this directory to put logos, big files, etc|
|scripts|lua scripts directory see [scripting section for more details](#scripting)|

### Special page extensions

|Extension|Description|
|---------|-----------|
|html|html files are not included in template, instead html itself is a template file|
|md|markdown are compiled to html and can be included in a template by including a variable(See more in Special Variables)|
|json|same as markdown|

Other extensions are tread as "raw" and are copied without any modification.

### Special variables

Variables are a feature from `inja` template engine, and you can use all default `inja` functions and variables, but cppaper include some platform specific to help you build templates

Note: The default one line statement `##` was changed to `%%` due to markdown template support to not conflict with markdown headings.

Generally you can use variables like this way:

```
<body>
  {{site.config.template}}
</body>
```

#### site

|Variable|Description|
|--------|-----------|
|config|Access all configs and its values|


#### Directory

|Variable|Description|
|--------|-----------|
|config|Access all configs and its values|

#### Page

|Variable|Description|
|--------|-----------|
|config|Access all configs and its values|
|title|Automatically title generated by cppaper|
|html|Markdown generated content (for .md page)|
|json|JSON variables (for .json page)|


### Special template functions

#### `getPagesFrom(path: string)`

return an array of pages given a project relative path directory


Example:

```
    {% for page in getPagesFrom(".") %}
      <a href="{{page.path}}">{{page.title}}</a>
    {% endfor %}
```

page structure:

```
{
    title: string,
    path: string,
    id: number
}
```

#### `getConfigFrom(id: number)`

return a key:value (typescript Record<string, string>) of configs from a file

Example:
```
  {% for page in getPagesFrom(".") %}
    {% set config = getConfigFrom(page.id) %}
    {{ config }}
  {% endfor %}
```

#### `getJsonFrom(id: number)`

return JSON object to be accessed on template

If file is not a JSON file, `null` is returned instead.

Example:
```
  {% for page in getPagesFrom(".") %}
    {% set json = getJsonFrom(page.id) %}
    {{ json.some_attribute }}
  {% endfor %}
```


#### `getPagesByTag(tag: string)`

return an ID array containing all pages marked with `tag`.

If no page contain this tag, an empty array is returned

NOTE: Different from `getPagesFrom` this only returns ID from pages objects(no titles or other attributes).

Example:

```
  {% for pageID in getPagesByTag("article") %} 
    {% set config = getConfigFrom(pageID) %}
    {{ config }}
  {% endfor %}
```

#### `lua(script: string)`

Return the result of the script.

Run lua directly from template

Example:

```
    {{ lua("return string.upper('thanks to lua super powers I became uppercased')") }}
```

This will produce the following string:


```
THANKS TO LUA SUPER POWERS I BECAME UPPERCASED
```

#### `getPagesWithConfig(config: string)`

return an entity ID array containing all pages with config `config` regardless the value (case sensitive).

Example:

```html
<h2>Pages with the config 'title'</h2>
<ul>
%% for page in getPagesWithConfig("title")
  <li>{{ getConfigFrom(page) }}: {{ getPathFrom(page) }}</li>
%% endfor
   <li></li>
</ul>
```

#### `getPagesWithConfig(config: string, value: string)`

return an entity ID array containing all pages with config `config` with the value `value`(case sensitve both for config and value).

Example:

```html
<h2>Pages with the config markdown_unsafe = 'true'</h2>
<ul>
%% for page in getPagesWithConfig("markdown_unsafe", "true")
  <li>{{ getConfigFrom(page) }}: {{ getPathFrom(page) }}</li>
%% endfor
  <li></li>
</ul>
```

## Scripting

Scripts is a very very(VERY!) experimental feature in cppaper, it allows you to run custom scripts.
Scripts ran after all files detected by cppaper, but there are some special functions you can specify to run on certain parts of the program, these are called `hooks`(see more below).
Every script has it own state, so if you can have a variable to be updated during all workflow of the program without losing any information.
No library is loaded by default(like base and package). So in order to some scripts to work you need to enable these libraries manually. See [lua_libraries in Site Config Section](#site)

### Special template functions

#### `getPagesWithConfig(config: string)`

return an entity ID array containing all pages with config `config` regardless the value (case sensitive).

Example:

```lua
for _, v in ipairs(get_pages_with_config('title')) do
    print(v)
end
```

#### `get_pages_with_config(config: string, value: string)`

return an entity ID array containing all pages with config `config` with the value `value`(case sensitve both for config and value).

Example:

```lua
for _, v in ipairs(get_pages_with_config('title', 'Custom Title')) do
  print(v)
end
```

### Hooks

Hooks are special functions called by `cppaper` on certain moment of the flow. For now there is only one.

#### `before_template`

This hook run before cppaper generates content based on templates.
In this hook you can manipulate json data and markdown html or other pages attributes to change template results.

#### `before_output`

This hook run before cppaper creating files to `public` directory.

## Docs

The docs are still in development, and is available [online in html version](https://cppaper.yurisantos.dev)

You also can compile the docs, that uses `cppaper` as engine:

`meson compile -C build docs`
