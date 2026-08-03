string_files = [
    '_split.c',
    '_utils.c',
    'stp_string.c',
]

string_headers = [
    '_utils.h',
    'stp_string.h',
]

stp_string_lib = static_library(
    'stp_string',
    [string_files, string_headers],
    include_directories: inc,
)
stp_string_dep = declare_dependency(
    link_with: stp_string_lib,
    include_directories: inc,
)
