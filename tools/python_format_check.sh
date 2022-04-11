#!/bin/bash
find src/python -name '*.py' | xargs yapf -d
