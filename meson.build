project('Steppable', 'c', version: '0.0.1')

cc = meson.get_compiler('c')

if get_option('buildtype') == 'debug'
    extra_args = []
    if cc.get_id() in ['gcc', 'clang']
        extra_args += '-Wpedantic'
    elif cc.get_id() == 'msvc'
        extra_args += '/permissive-'
    endif
    add_project_arguments(extra_args, language: 'c')
endif

m_dep = cc.find_library('m', required : true)

if cc.get_id() not in ['msvc', 'clang-cl']
    m_dep = cc.find_library('m', required : true)
else
    m_dep = dependency('', required : false)
endif

inc = include_directories([
    'include',
    'stp_number',
    'stp_cmplx',
    'stp_string',
    'stn_serialize',
])

subdir('stp_string')
subdir('stp_number')
subdir('stn_serialize')
subdir('stp_cmplx')
subdir('tests')
