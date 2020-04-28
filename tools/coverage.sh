#!/bin/bash
cd ..
rm ./*.info
rm -rf ./out/coverage

lcov -b . -c -d out -o coverage.info
lcov -e coverage.info "`pwd`/*" -o coverage_filtered.info
lcov -r coverage_filtered.info "`pwd`/out/*.*" -o coverage_filtered.info
lcov -r coverage_filtered.info "`pwd`/tests/*.*" -o coverage_filtered.info
lcov -r coverage_filtered.info "`pwd`/src/*Logger.*" -o coverage_filtered.info
genhtml -o out/coverage coverage_filtered.info
