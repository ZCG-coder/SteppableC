number_files = [
    '_add.c',
    '_alloc.c',
    '_cmp.c',
    '_div.c',
    '_newtondiv.c',
    '_knuth.c',
    '_exp.c',
    '_karatsuba.c',
    '_mul.c',
    '_print.c',
    '_log.c',
    '_round.c',
    '_scale.c',
    '_sqrt.c',
    '_shift.c',
    '_sqr.c',
    '_sub.c',
    'stp_number.c',
]

number_headers = [
    'stp_number.h',
    '_div.h',
    '_mul.h',
    '_shift.h',
    '_utils.h',
]

stp_number_lib = static_library(
    'stp_number',
    [number_files, number_headers],
    include_directories: inc,
    dependencies: stp_string_dep,
)

stp_number_dep = declare_dependency(
    link_with: stp_number_lib,
    include_directories: inc,
)
