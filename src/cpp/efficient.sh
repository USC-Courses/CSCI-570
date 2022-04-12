#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")
g++ -O2 -std=c++17 -o "${WDIR}/efficient" "${WDIR}/efficient.cpp"
if [ ${WDIR:0:1} == '/' ]; then
    "${WDIR}/efficient" "$1" "$2"
else
    ./"${WDIR}/efficient" "$1" "$2"
fi
