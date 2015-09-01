#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt

# num processes,num networks,last generation,time,games,plays,passes


def plot_time(data, out):
    speedup = data[0, 3] / data[:, 3]

    fig = plt.figure()

    # speedup per process
    ax_sp = fig.add_subplot(1, 1, 1)
    ax_sp.plot(data[:, 0], speedup, 'x')
    ax_sp.set_title("Speedup")
    ax_sp.set_xlabel("# Processes")
    ax_sp.set_ylabel("Speedup")
    ax_sp.legend(["measurements"], loc='best')

    fig.tight_layout()
    fig.savefig(out)


def main():
    if len(sys.argv) < 2:
        print("need csv file as cli parameter")
        sys.exit(1)

    data = np.loadtxt(sys.argv[1], delimiter=',')

    plot_time(data, 'weak_scaling_time.pdf')


if __name__ == '__main__':
    main()
