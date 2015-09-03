#!/usr/bin/env python3

# import sys
import numpy as np
import matplotlib.pyplot as plt


def plot_time(data, out):
    speedup = [d[1][0, 2] / d[1][:, 2] for d in data]
    efficiency = [s / d[1][:, 0] for d, s in zip(data, speedup)]

    fig = plt.figure()

    # speedup per process
    ax_sp = fig.add_subplot(2, 1, 1)
    for d, s in zip(data, speedup):
        ax_sp.plot(d[1][:, 0], s, 'x')
    ax_sp.plot(data[0][1][:, 0], data[0][1][:, 0])
    ax_sp.set_title("Speedup")
    ax_sp.set_xlabel("# Processes")
    ax_sp.set_ylabel("Speedup")
    legend = [d[0] for d in data]
    legend.append('ideal speedup')
    ax_sp.legend(legend, loc='best')

    # efficiency per process
    ax_ef = fig.add_subplot(2, 1, 2)
    for d, e in zip(data, efficiency):
        ax_ef.plot(d[1][:, 0], e, 'x')
    ax_ef.set_title("Efficiency")
    ax_ef.set_xlabel("# Processes")
    ax_ef.set_ylabel("Efficiency")
    legend = [d[0] for d in data]
    ax_ef.legend(legend, loc='best')

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
    # if len(sys.argv) < 2:
    #     print("need csv file as cli parameter")
    #     sys.exit(1)

    data = list()
    data.append(('static', np.loadtxt('strong_scaling.csv', delimiter=',')))
    data.append(('10-0.5', np.loadtxt('strong_scaling_dynsched-10-05.csv',
                                      delimiter=',')))
    data.append(('5-0.5', np.loadtxt('strong_scaling_dynsched-5-05.csv',
                                     delimiter=',')))
    data.append(('1-0.5', np.loadtxt('strong_scaling_dynsched-1-05.csv',
                                     delimiter=',')))
    # data.append(('2-0.25', np.loadtxt('strong_scaling_dynsched-2-025.csv',
    #                                  delimiter=',')))

    data2 = list()
    data2.append(('static', np.loadtxt('strong_scaling.csv', delimiter=',')))
    data2.append(('1-0.50', np.loadtxt('strong_scaling_dynsched-1-05.csv',
                                       delimiter=',')))
    data2.append(('1-0.75', np.loadtxt('strong_scaling_dynsched-1-075.csv',
                                       delimiter=',')))
    data2.append(('1-0.25', np.loadtxt('strong_scaling_dynsched-1-025.csv',
                                       delimiter=',')))
    data2.append(('1-0.00', np.loadtxt('strong_scaling_dynsched-1-00.csv',
                                       delimiter=',')))
    # data2.append(('1-0.375',
    #               np.loadtxt('strong_scaling_dynsched-1-0375.csv', delimiter=',')))
    # data2.append(('1-0.125',
    #               np.loadtxt('strong_scaling_dynsched-1-0125.csv', delimiter=',')))
    # data2.append(('1-0.33',
    #               np.loadtxt('strong_scaling_dynsched-1-033.csv', delimiter=',')))

    plot_time(data, 'strong_scaling_time_chunksize.pdf')
    plot_time(data2, 'strong_scaling_time_initial.pdf')
    # plot_moves(data, 'strong_scaling_moves.pdf')
    # plot_time(data2, 'strong_scaling_time_dynsched-10-05.pdf', '10-0.5')
    # plot_moves(data2, 'strong_scaling_moves_dynsched-10-05.pdf')
    # plot_time(data3, 'strong_scaling_time_dynsched-5-05.pdf', '5-0.5')
    # plot_moves(data3, 'strong_scaling_moves_dynsched-5-05.pdf')


if __name__ == '__main__':
    main()
