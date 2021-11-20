#!/usr/bin/env python3
import sys
from typing import List

SPACES_PER_TAB = 8


def main():
    with open(sys.argv[1], "r", encoding="utf-8") as file:
        in_lines = [line.rstrip() for line in file.readlines()]
    out_lines: List[str] = []
    for line in in_lines:
        pos = 0
        out_lines.append("")
        for char in line:
            if char == "\t":
                filled = pos % SPACES_PER_TAB
                to_be_added = SPACES_PER_TAB - filled
                out_lines[-1] += to_be_added * " "
                pos += to_be_added
            else:
                out_lines[-1] += char
                pos += 1

    print("\n".join(out_lines))


if __name__ == "__main__":
    main()
