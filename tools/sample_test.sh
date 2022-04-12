#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")/../
if [ ${WDIR:0:1} != "/" ]; then
    WDIR=$DIR/$WDIR
fi
cd "$WDIR"

function run_test {
    echo "--------------------------------------------------"
    language=$1
    script_dir="src/${language}"
    data_dir="data/SampleTestCases"
    for input_file in ${data_dir}/input*.txt; do
        output_file=${input_file/input/output}
        filename=${input_file##*/}
        no=${filename//[^0-9]/}

        bash ${script_dir}/basic.sh ${input_file} ${data_dir}/${language}_basic${no}.out &>/dev/null
        diff ${output_file} ${data_dir}/${language}_basic${no}.out
        if [ $? != 1 ]; then
            echo "${language} basic program test ${no}: ${filename} passed"
        else
            echo "${language} basic program test ${no}: ${filename} failed"
        fi

        bash ${script_dir}/efficient.sh ${input_file} ${data_dir}/${language}_efficient${no}.out &>/dev/null
        diff ${output_file} ${data_dir}/${language}_efficient${no}.out
        if [ $? != 1 ]; then
            echo "${language} efficient program test ${no}: ${filename} passed"
        else
            echo "$language efficient program test ${no}: $filename failed"
        fi
    done
    echo "--------------------------------------------------"
}

echo 'Test C++'
run_test "cpp"

echo 'Test Java'
run_test "java"

echo 'Test Python'
run_test "python"

cd "$DIR"
