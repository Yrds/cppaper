project('cppaper', 'cpp', version: '0.5.0', default_options: ['cpp_std=c++20', 'default_library=static'])


src = files(
  'src/systems/template.cpp',
  'src/systems/config.cpp',
  'src/systems/title.cpp',
  'src/cppaper.cpp',
  )

inc = include_directories('src')

cmark_gfm_sub = subproject('cmark-gfm', default_options: 'default_library=static')

inja_sub = subproject('inja')

entt_sub = subproject('entt')



cppaper_exe = executable('cppaper', src, include_directories: inc, dependencies: [
  cmark_gfm_sub.get_variable('cmark_gfm_dep'),
  inja_sub.get_variable('inja_dep'),
  entt_sub.get_variable('entt_dep')
  ],
  install_dir: '/bin/',
  install: false,
  override_options: ['cpp_std=c++20', 'default_library=static']
  )

generate_docs = run_target('docs', depends:[cppaper_exe],
  command: ['generate_docs.sh'])
