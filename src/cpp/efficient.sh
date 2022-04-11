#!/bin/bash
g++ -O2 -std=c++17 -o efficient efficient.cpp
./efficient "$1" "$2"
