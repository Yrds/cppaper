project('blogcpp', 'cpp', version: '0.0.1', default_options: ['cpp_std=c++20'])

src = files(
  'src/main.cpp'
  )

inc = include_directories('src')

cppmark_subproj = subproject('cppmark')

dependencies = []
libcppmark_dep = cppmark_subproj.get_variable('libcppmark_dep')
dependencies += libcppmark_dep

executable('blogcpp', src, include_directories: inc, dependencies: dependencies )


