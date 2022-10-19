#!/usr/bin/env python3

import argparse
import subprocess
import pathlib
import re


def _load_parser():
    p = argparse.ArgumentParser()
    p.add_argument("executable", type=pathlib.Path)
    return p


def texify_console(lines):
    def normalize(l):
        for old, new in ((" {", r" \{"), ("} ", r"\} "), ("\x1B[0m", "}"), ("\x1B[31m", r"\RD{"), ("\x1B[32m", r"\GR{"), ("├", r"\textSFviii{}"), ("│", r"\textSFxi{}"), ("└", r"\textSFii{}")):
            l = l.replace(old, new)
            if old[0] == ' ' and l.startswith(old.strip()):
                l = f'{new.strip()}{l[len(old.strip()):]}'
            if old[-1] == ' ' and l.endswith(old.strip()):
                l = f'{l[:-len(old.strip())]}{new.strip()}'
        l = re.sub("/[-.a-zA-Z/0-9]+:", "demo.cpp:", l)
        if 'segmentation fault' in l:
            l = fr'\RD{{{l}}}'
        return l

    result, repeat = [], ''
    for l in (normalize(l) for l in lines):
        l = f'{repeat}{l}'
        repeat = ''

        # Repeat multi-line color instructions
        if l[0] == "\\" and len(re.findall('}', l)) == 0:
            m = re.match(r'(\\[A-Z]+{)', l)
            if m:
                repeat = m.groups()[0]
            l += '}'

        # Overlong line handling
        add = [l]
        if len(l) > 100:
            add = list(l.split(' <= '))
            add = [add[0]] + [f' <= {a}' for a in add[1:]]
        result.extend(add)
    return result


def capture_run(executable: pathlib.Path):
    result = subprocess.run([str(executable), "--color=always"], capture_output=True, encoding="utf-8")
    lines = result.stdout.splitlines()
    if result.returncode == -11:
        lines.append(r"segmentation fault (core dumped)")
    return lines


def _main():
    args = vars(_load_parser().parse_args())
    output = capture_run(**args)
    output = texify_console(output)
    print("\n".join(output))


if __name__ == "__main__":
    _main()
