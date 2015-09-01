#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt


def plot_time(data, out):
    speedup = data[0, 2] / data[:, 2]
    efficiency = speedup / data[:, 0]

    plt.close()

    # speedup per process
    plt.subplot(2, 1, 1)
    plt.plot(data[:, 0], speedup, 'x')
    plt.plot(data[:, 0], data[:, 0])
    plt.title("Speedup")
    plt.xlabel("# Processes")
    plt.ylabel("Speedup")
    plt.legend(["measurements", "ideal speedup"], loc='best')

    # efficiency per process
    plt.subplot(2, 1, 2)
    plt.plot(data[:, 0], efficiency, 'x')
    plt.title("Efficiency")
    plt.xlabel("# Processes")
    plt.ylabel("Efficiency")
    plt.legend(["measurements"], loc='best')

    plt.tight_layout()
    plt.savefig(out, papertype='a4')


def plot_moves(data, out):
    moves = data[0, 4] + data[0, 5]
    moves_per_s = moves / data[:, 2]

    plt.close()

    # moves/s
    plt.plot(data[:, 0], moves_per_s, 'x')
    plt.plot(data[:, 0], moves_per_s[0] * data[:, 0])
    plt.title("moves/s")
    plt.xlabel("# Processes")
    plt.ylabel("Moves/s")
    plt.legend(["measurements", "linear"], loc='best')

    plt.tight_layout()
    plt.savefig(out, papertype='a4')


def main():
    if len(sys.argv) < 2:
        print("need csv file as cli parameter")
        sys.exit(1)

    data = np.loadtxt(sys.argv[1], delimiter=',')

    plot_time(data, 'strong_scaling_time.pdf')
    plot_moves(data, 'strong_scaling_moves.pdf')


if __name__ == '__main__':
    main()
