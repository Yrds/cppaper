project('cppaper', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++20'])


src = files(
  'src/cppaper.cpp',
  'src/template.cpp'
  )

inc = include_directories('src')

cmark_gfm_sub = subproject('cmark-gfm')

inja_sub = subproject('inja')


executable('cppaper', src, include_directories: inc, dependencies: [
  cmark_gfm_sub.get_variable('cmark_gfm_dep'),
  inja_sub.get_variable('inja_dep')
  ],
  install_rpath: '$ORIGIN/../lib',
  install_dir: '/bin/',
  install: true,
  override_options: ['cpp_std=c++20']
  )

install_data('build/subprojects/cmark-gfm-0.29.0.gfm.6/libcmark-gfm.so', install_dir: '/lib/')
