#!/bin/bash
INPLACE=0
while [[ $# -gt 0 ]]; do
    case "${1}" in
    -i | --in-place) INPLACE=1 ;;
    -h | --help)
        echo "Usage: $(basename "$0") [-i --in-place]"
        exit 0
        ;;
    *)
        echo "ERROR: Unrecognized argument: $1"
        echo "Usage: $(basename "$0") [-i --in-place]"
        exit 1
        ;;
    esac
    shift
done

pushd $(pwd) >/dev/null
cd $(dirname "$0")/../

if [ ${INPLACE} -eq 0 ]; then
    find src/python plot/ -name '*.py' | xargs yapf -d
else
    find src/python plot/ -name '*.py' | xargs yapf -i
fi

popd >/dev/null
