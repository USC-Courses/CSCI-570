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

        basic_output=${data_dir}/${language}_basic${no}.out
        bash ${script_dir}/basic.sh ${input_file} ${basic_output} &>/dev/null
        if [ -e ${basic_output} ]; then
            diff ${output_file} ${basic_output}
            if [ $? != 1 ]; then
                echo -e "${language} basic     program test ${no}: ${filename} passed"
            else
                echo -e "${language} basic     program test ${no}: ${filename} failed"
            fi
        else
            echo -e "${language} basic     program test ${no}: ${filename} ignored"
        fi

        efficient_output=${data_dir}/${language}_efficient${no}.out
        bash ${script_dir}/efficient.sh ${input_file} ${efficient_output} &>/dev/null
        if [ -e ${efficient_output} ]; then
            diff ${output_file} ${efficient_output}
            if [ $? != 1 ]; then
                echo -e "${language} efficient program test ${no}: ${filename} passed"
            else
                echo -e "${language} efficient program test ${no}: ${filename} failed"
            fi
        else
            echo -e "${language} efficient program test ${no}: ${filename} ignored"
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
