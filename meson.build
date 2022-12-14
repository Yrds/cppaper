project('cppaper', 'cpp', version: '0.2.1', default_options: ['cpp_std=c++20', 'default_library=static'])


src = files(
  'src/cppaper.cpp',
  'src/template.cpp',
  'src/config.cpp',
  'src/systems/template.cpp'
  )

inc = include_directories('src')

cmark_gfm_sub = subproject('cmark-gfm', default_options: 'default_library=static')

inja_sub = subproject('inja')

entt_sub = subproject('entt')


executable('cppaper', src, include_directories: inc, dependencies: [
  cmark_gfm_sub.get_variable('cmark_gfm_dep'),
  inja_sub.get_variable('inja_dep'),
  entt_sub.get_variable('entt_dep')
  ],
  install_rpath: '$ORIGIN/../lib',
  install_dir: '/bin/',
  install: true,
  override_options: ['cpp_std=c++20', 'default_library=static']
  )
