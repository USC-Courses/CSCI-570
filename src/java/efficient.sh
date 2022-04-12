#!/bin/bash
DIR=$(pwd)
WDIR=$(dirname "$0")
javac "${WDIR}/Efficient.java"
java --class-path "${WDIR}" Efficient "$1" "$2"
