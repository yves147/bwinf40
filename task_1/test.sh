#!/bin/bash
set -euo pipefail
IFS=$' \n\t'
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

g++ -Wall -Wextra -Wshadow -fsanitize=undefined,address -D_GLIBCXX_DEBUG -g main.cpp
echo done
cat ${DIR}/examples/parkplatz0.txt | ./a.out

