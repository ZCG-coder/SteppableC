number_files = [
    '_add.c',
    '_alloc.c',
    '_cmp.c',
    '_div.c',
    '_karatsuba.c',
    '_mul.c',
    '_print.c',
    '_scale.c',
    '_shift.c',
    '_sub.c',
    'stp_number.c',
]

executable(
    'stp_number_dbg',
    number_files,
    include_directories: inc,
    dependencies: stp_string_dep,
)

stp_number_lib = library(
    'stp_number',
    number_files,
    include_directories: inc,
    dependencies: stp_string_dep,
)

stp_number_dep = declare_dependency(
    link_with: stp_number_lib,
    include_directories: inc,
)
