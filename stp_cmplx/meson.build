cmplx_files = [
    'stp_cmplx.c',
    '_abs.c',
    '_conjg.c',
    '_cmpl_arithmetic.c',
]

cmplx_headers = [
    'stp_cmplx.h',
]

cmplx = static_library(
    'stp_cmplx',
    [cmplx_files, cmplx_headers],
    include_directories: inc,
    dependencies: [stp_number_dep, stp_string_dep, m_dep],
)

stp_cmplx_dep = declare_dependency(
    link_with: cmplx,
    include_directories: inc,
)
