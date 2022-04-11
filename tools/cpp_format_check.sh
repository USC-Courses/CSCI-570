#!/bin/bash
find src/cpp -name '*.cpp' | xargs clang-format --Werror --dry-run
