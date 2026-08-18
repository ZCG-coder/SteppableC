conf_data = configuration_data()

conf_data.set_quoted('VERSION', meson.project_version())

# test for int128
if (cc.sizeof('__int128_t', prefix : '#include<stdint.h>') == 16 \
    or cc.sizeof('int128_t', prefix : '#include<stdint.h>') == 16 \
    or cc.sizeof('int128', prefix : '#include<stdint.h>') == 16) and \
   (cc.sizeof('__uint128_t', prefix : '#include<stdint.h>') == 16 \
    or cc.sizeof('uint128_t', prefix : '#include<stdint.h>') == 16 \
    or cc.sizeof('uint128', prefix : '#include<stdint.h>') == 16)
    conf_data.set10('INT128_ENABLED', true)
endif

# test for MSVC
if (cc.get_id() == 'msvc')
    conf_data.set('MSVC', true)
endif

configure_file(
    input: 'config.h.in',
    output: 'config.h',
    configuration: conf_data
)
