#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt


def plot_time(data, out):
    speedup = [d['data'][0, 2] / d['data'][:, 2] for d in data]
    efficiency = [s / d['data'][:, 0] for d, s in zip(data, speedup)]

    fig = plt.figure(figsize=(12, 5))

    # speedup per process
    ax_sp = fig.add_axes([0.075, 0.55, 0.75, 0.375])
    ax_sp.plot(data[0]['data'][:, 0], data[0]['data'][:, 0],
               label='ideal', color='grey')
    for d, s in zip(data, speedup):
        ax_sp.plot(d['data'][:, 0], s, '+', label=d['label'], color=d['color'])
    ax_sp.set_ylabel("Speedup")
    h_sp, l_sp = ax_sp.get_legend_handles_labels()
    ax_sp.tick_params(labelright=True)

    # efficiency per process
    ax_ef = fig.add_axes([0.075, 0.1, 0.75, 0.375])
    for d, e in zip(data, efficiency):
        ax_ef.plot(d['data'][:, 0], e, '+', color=d['color'])
    ax_ef.set_xlabel("# Processes")
    ax_ef.set_ylabel("Efficiency")
    h_ef, l_ef = ax_ef.get_legend_handles_labels()
    ax_ef.tick_params(labelright=True)

    fig.legend(h_sp + h_ef, l_sp + l_ef, loc=(0.875, 0.1))

    # fig.tight_layout()
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
    data = list()
    data.append({
        'label': 'static',
        'color': 'blue',
        'data': np.loadtxt('strong_scaling.csv', delimiter=','),
    })
    plot_time(data, 'strong_scaling_time_static.pdf')
    data.append({
        'label': '1-0.5',
        'color': 'red',
        'data': np.loadtxt('strong_scaling_dynsched-1-05.csv', delimiter=','),
    })
    data.append({
        'label': '5-0.5',
        'color': 'orange',
        'data': np.loadtxt('strong_scaling_dynsched-5-05.csv', delimiter=','),
    })
    data.append({
        'label': '10-0.5',
        'color': 'green',
        'data': np.loadtxt('strong_scaling_dynsched-10-05.csv', delimiter=','),
    })
    # data.append(('2-0.25', np.loadtxt('strong_scaling_dynsched-2-025.csv',
    #                                  delimiter=',')))

    data2 = list()
    data2.append({
        'label': 'static',
        'color': 'blue',
        'data': np.loadtxt('strong_scaling.csv', delimiter=','),
    })
    data2.append({
        'label': '1-0.25',
        'color': 'red',
        'data': np.loadtxt('strong_scaling_dynsched-1-025.csv', delimiter=','),
    })
    plot_time(data2, 'strong_scaling_time_final.pdf')
    data2.append({
        'label': '1-0.50',
        'color': 'orange',
        'data': np.loadtxt('strong_scaling_dynsched-1-05.csv', delimiter=','),
    })
    data2.append({
        'label': '1-0.75',
        'color': 'green',
        'data': np.loadtxt('strong_scaling_dynsched-1-075.csv', delimiter=','),
    })
    # data2.append(('1-0.00', np.loadtxt('strong_scaling_dynsched-1-00.csv',
    #                                    delimiter=','), 'grey'))
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
