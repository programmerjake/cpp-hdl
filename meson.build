project('cpp-hdl', 'cpp', default_options : ['cpp_std=c++14'], license : 'GPL3+')
thread_dep = dependency('threads')
src = [
    'main.cpp',
    'parse_error.cpp',
    'source.cpp',
    'tokenizer.cpp']
executable('cpp-hdl', src, dependencies : thread_dep)

