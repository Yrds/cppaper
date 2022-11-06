project('cppaper', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++20'])


src = files(
  'src/cppaper.cpp'
  )

inc = include_directories('src')

cmark_gfm_sub = subproject('cmark-gfm')

executable('cppaper', src, include_directories: inc, dependencies: [
  cmark_gfm_sub.get_variable('cmark_gfm_dep')
  ])
