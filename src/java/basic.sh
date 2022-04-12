#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")
javac "${WDIR}/Basic.java"
java --class-path "${WDIR}" Basic "$1" "$2"
