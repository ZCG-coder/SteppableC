string_files = [
    '_split.c',
    '_utils.c',
    'stp_string.c',
]

stp_string_lib = static_library('stp_string', string_files, include_directories: inc)
stp_string_dep = declare_dependency(
    link_with: stp_string_lib,
    include_directories: inc,
)
