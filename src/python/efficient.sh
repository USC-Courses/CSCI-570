#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")
if [ ${WDIR:0:1} == '/' ]; then
    python3 "${WDIR}/efficient_3.py" "$1" "$2"
else
    python3 ./"${WDIR}/efficient_3.py" "$1" "$2"
fi
