project('cpp-hdl', 'cpp', default_options : ['cpp_std=c++14'], license : 'GPL3+')
thread_dep = dependency('threads')
gmp_dep = meson.get_compiler('cpp').find_library('gmp')
src = [
    'ast/bit_vector_type.cpp',
    'ast/bundle.cpp',
    'ast/dump_visitor.cpp',
    'ast/module.cpp',
    'ast/module_template.cpp',
    'ast/module_template_argument.cpp',
    'ast/module_template_parameter_kind.cpp',
    'ast/node.cpp',
    'ast/symbol_table.cpp',
    'ast/template_parameter.cpp',
    'ast/transparent_type_alias.cpp',
    'ast/type_pool.cpp',
    'ast/value_template_argument.cpp',
    'ast/value_template_parameter_kind.cpp',
    'ast/variable.cpp',
    'main.cpp',
    'output_generator.cpp',
    'parse_error.cpp',
    'parser.cpp',
    'source.cpp',
    'tokenizer.cpp',
    'verilog_output_generator.cpp']
executable('cpp-hdl', src, dependencies : [thread_dep, gmp_dep])

