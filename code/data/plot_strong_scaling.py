#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt


def plot_time(data, out):
    speedup = data[0, 2] / data[:, 2]
    efficiency = speedup / data[:, 0]

    fig = plt.figure()
    # fig.suptitle("Strong Scaling")

    # speedup per process
    ax_sp = fig.add_subplot(2, 1, 1)
    ax_sp.plot(data[:, 0], speedup, 'x')
    ax_sp.plot(data[:, 0], data[:, 0])
    ax_sp.set_title("Speedup")
    ax_sp.set_xlabel("# Processes")
    ax_sp.set_ylabel("Speedup")
    ax_sp.legend(["measurements", "ideal speedup"], loc='best')

    # efficiency per process
    ax_ef = fig.add_subplot(2, 1, 2)
    ax_ef.plot(data[:, 0], efficiency, 'x')
    ax_ef.set_title("Efficiency")
    ax_ef.set_xlabel("# Processes")
    ax_ef.set_ylabel("Efficiency")
    ax_ef.legend(["measurements"], loc='best')

    fig.tight_layout()
    fig.savefig(out, papertype='a4')


def plot_moves(data, out):
    moves = data[0, 4] + data[0, 5]
    moves_per_s = moves / data[:, 2]

    fig = plt.figure()

    # moves/s
    ax_mo = fig.add_subplot(1, 1, 1)
    ax_mo.plot(data[:, 0], moves_per_s, 'x')
    ax_mo.plot(data[:60, 0], moves_per_s[0] * data[:60, 0])
    ax_mo.set_title("Moves/s")
    ax_mo.set_xlabel("# Processes")
    ax_mo.set_ylabel("Moves/s")
    ax_mo.legend(["measurements", "ideal"], loc='best')

    fig.tight_layout()
    fig.savefig(out, papertype='a4')


def main():
    if len(sys.argv) < 2:
        print("need csv file as cli parameter")
        sys.exit(1)

    data = np.loadtxt(sys.argv[1], delimiter=',')

    plot_time(data, 'strong_scaling_time.pdf')
    plot_moves(data, 'strong_scaling_moves.pdf')


if __name__ == '__main__':
    main()
