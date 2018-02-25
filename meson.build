project('cpp-hdl', 'cpp', default_options : ['cpp_std=c++14'], license : 'GPL3+')
thread_dep = dependency('threads')
gmp_dep = meson.get_compiler('cpp').find_library('gmp')
src = [
    'ast.cpp',
    'main.cpp',
    'output_generator.cpp',
    'parse_error.cpp',
    'parser.cpp',
    'source.cpp',
    'tokenizer.cpp',
    'verilog_output_generator.cpp']
executable('cpp-hdl', src, dependencies : [thread_dep, gmp_dep])

