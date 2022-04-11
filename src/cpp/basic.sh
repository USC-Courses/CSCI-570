#!/bin/bash
g++ -O2 -std=c++17 -o basic basic.cpp
./basic "$1" "$2"
