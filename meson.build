project('cppaper', 'cpp', version: '0.25.0', default_options: ['cpp_std=c++20', 'default_library=static'])

src = files(
  #Utils
  'src/string_utils.cpp',

  'src/systems/template.cpp',
  'src/systems/lua.cpp',
  'src/systems/config.cpp',
  'src/systems/title.cpp',
  'src/systems/index.cpp',
  'src/systems/json.cpp',
  'src/systems/directoriesMap.cpp',
  'src/systems/relativePath.cpp',
  'src/systems/extension.cpp',
  'src/systems/markdown.cpp',
  'src/systems/tag.cpp',
  'src/systems/fileContent.cpp',
  'src/systems/sitemap.cpp',
  'src/systems/output.cpp',
  'src/cppaper.cpp',

  #Template functions
  'src/template/functions/getPagesFrom.cpp',
  'src/template/functions/getConfigFrom.cpp',
  'src/template/functions/getJsonFrom.cpp',
  'src/template/functions/getPagesByTag.cpp',
  'src/template/functions/getPathFrom.cpp',
  'src/template/functions/lua.cpp'
  )

inc = include_directories('src')

cmark_gfm_sub = subproject('cmark-gfm', default_options: 'default_library=static')

inja_sub = subproject('inja')

entt_sub = subproject('entt')

deps = []

deps += cmark_gfm_sub.get_variable('cmark_gfm_dep')
deps += inja_sub.get_variable('inja_dep')
deps += entt_sub.get_variable('entt_dep')
deps += subproject('sol2').get_variable('sol2_dep')
deps += subproject('tfstring').get_variable('tfstring_dep')
deps += subproject('lua').get_variable('lua_dep')

cppaper_exe = executable('cppaper', src, include_directories: inc,
  dependencies: deps, install_dir: '/usr/bin/', install: true,
  )

generate_docs = run_target('docs', depends:[cppaper_exe],
  command: ['generate_docs.sh'])

#TODO benchmark
#bm_markdown_250 = benchmark('markdown_250', 'cppaper', args: ['-C', './bench'] depends: [cppaper_exe])
