#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt

# num processes,num networks,last generation,time,games,plays,passes


def plot_time(data, out):
    speedup = [d[1][0, 3] / d[1][:, 3] for d in data]

    fig = plt.figure()

    # speedup per process
    ax_sp = fig.add_subplot(1, 1, 1)
    for d, s in zip(data, speedup):
        ax_sp.plot(d[1][:, 0], s, 'x')
    ax_sp.set_title("Speedup")
    ax_sp.set_xlabel("# Processes")
    ax_sp.set_ylabel("Speedup")
    legend = [d[0] for d in data]
    ax_sp.legend(legend, loc='best')

    fig.tight_layout()
    fig.savefig(out)


def main():
    # if len(sys.argv) < 2:
    #     print("need csv file as cli parameter")
    #     sys.exit(1)
    data = list()

    data.append(('static', np.loadtxt('weak_scaling.csv', delimiter=',')))
    data.append(('1-0.25', np.loadtxt('weak_scaling_dynsched.csv', delimiter=',')))

    plot_time(data, 'weak_scaling_time.pdf')


if __name__ == '__main__':
    main()
