test_cases = [
    'add',
    'sub',
    'mul',
    'div',
    'log',
    'self_add',
    'self_sub',
    'self_mul',
    'self_div',
    'sqr',
    'exp',
    'string',
    'print',
]

foreach test_case : test_cases
    test_case_name = 'test_' + test_case
    file_name = 'test_' + test_case + '.c'

    exe = executable(
        test_case_name,
        file_name,
        dependencies: [stp_number_dep, stp_string_dep]
    )

    test(
            test_case_name,
            exe
        )

endforeach
