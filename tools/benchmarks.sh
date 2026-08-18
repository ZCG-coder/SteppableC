#!/bin/sh

if ! command -v hyperfine >/dev/null 2>&1
then
    echo "hyperfine could not be found" >&2
fi

mkdir -p ./benchmarks

# benchmark targets
array=(
    'test_stp_number_exp "test_e_generation"'
    'test_stp_number_pi'
    'test_stp_number_pi'
    'test_stp_number_sqr'
    'test_stp_number_sqrt'
)
for i in "${array[@]}"; do
    echo "== BENCHMARKING $i"

    j=$(echo $i | head -n1 | cut -d " " -f1)
    hyperfine --warmup 2 --runs 10 "tests/$i" -n $j --export-csv "benchmarks/$j.csv" -u millisecond
done
