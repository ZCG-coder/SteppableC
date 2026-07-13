test_cases = [
    'add',
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
    test(test_case_name, exe)
endforeach
