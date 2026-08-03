serialize_files = [
    '_serialize.c',
    '_deserialize.c',
]

serialize_headers = [
    '_utils.h',
    'stn_serialize.h',
]

stn_serialize = static_library(
    'serialize',
    [serialize_files, serialize_headers],
    include_directories: inc,
    dependencies: stp_number_dep,
)

stn_serialize_dep = declare_dependency(
    link_with: stn_serialize,
    include_directories: inc,
)
