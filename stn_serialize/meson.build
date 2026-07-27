serialize_files = [
    '_serialize.c',
    '_deserialize.c',
]

stn_serialize = static_library(
    'serialize',
    serialize_files,
    include_directories: inc,
    dependencies: stp_number_dep,
)

stn_serialize_dep = declare_dependency(
    link_with: stn_serialize,
    include_directories: inc,
)
