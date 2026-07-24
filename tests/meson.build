test_cases = [
    'add',
    'sub',
    'mul',
    'div',
    'self_add',
    'self_sub',
    'self_mul',
    'self_div',
    'sqr',
    'exp',
    'string',
]

foreach test_case : test_cases
    test_case_name = 'test_' + test_case
    file_name = 'test_' + test_case + '.c'

    exe = executable(
        test_case_name,
        file_name,
        dependencies: [stp_number_dep, stp_string_dep]
    )

    if host_machine.system() == 'darwin'
        if leaks_util.found()
            leaks_util_path = leaks_util.full_path()
            leak_env = ['MallocStackLogging=1']

            test(
                '(leak test) ' + test_case_name,
                leaks_util,
                args: ['--atExit', '--', exe],
                env: leak_env
            )
        endif
    else
        test(
                test_case_name,
                exe
            )
    endif

endforeach
