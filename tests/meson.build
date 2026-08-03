test_suites = [
    'stp_number',
    'stp_string',
    'stn_serialize',
]

foreach test_suite : test_suites
    test_suite_name = 'test_' + test_suite

    subdir(test_suite_name)
    foreach test_case : test_cases
        file_name = 'test_' + test_case + '.c'
        test_case_name = 'test_' + test_suite + '_' + test_case

        exe = executable(
            test_case_name,
            [test_suite_name / file_name, '_utils.h'],
            dependencies: [stp_number_dep, stp_string_dep, stn_serialize_dep],
        )
        test(
            test_case_name,
            exe,
            suite: [test_suite],
        )
    endforeach
endforeach
