#!/usr/bin/env python3

import subprocess
import sys


def main():
    if len(sys.argv) < 2:
        print("Usage: ./flops.py <executable>")
        return

    call = [
        'perf', 'stat',
        # '-x', ',',
        '-e', 'r530110',
        '-e', 'r531010',
        '-e', 'r532010',
        '-e', 'r534010',
        '-e', 'r538010'
    ]
    call.append(sys.argv[1])

    # out = subprocess.check_output(call, stdout=subprocess.DEVNULL)
    p = subprocess.Popen(call, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    out = p.stderr.read().decode()
    print(out)
    # for l in out.splitlines():
    #     print(l.split(','))


if __name__ == '__main__':
    main()
