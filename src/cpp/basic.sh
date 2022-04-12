#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")
g++ -O2 -std=c++17 -o "${WDIR}/basic" "${WDIR}/basic.cpp"
if [ ${WDIR:0:1} == '/' ]; then
    "${WDIR}/basic" "$1" "$2"
else
    ./"${WDIR}/basic" "$1" "$2"
fi
