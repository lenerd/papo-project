#!/usr/bin/env python3

import sys
import os
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import spline


def load_data(filename):
    if os.path.exists(filename + '.csmooth'):
        return np.loadtxt(filename + '.csmooth', delimiter=',')
    data = np.loadtxt(filename, delimiter=',')
    data_smooth = smooth_data(data)
    np.savetxt(filename + '.csmooth', data_smooth, delimiter=',')
    return data_smooth


def smooth_data(data):
    xnew = np.linspace(0, 999, 250)
    return spline(range(1000), data, xnew)



def main():
    if len(sys.argv) < 2:
        print("csv file as argument", file=sys.stderr)
        sys.exit(1)

    # 0: generation
    # 1: time
    # 2: plays
    # 3: passes
    data = load_data(sys.argv[1])
    #csd = np.cumsum(data, axis=0)

    #xnew = np.linspace(0, 999, 250)
    #data_smooth = spline(range(1000), data, xnew)
    data_smooth = data

    x_smooth = data_smooth[:,0]
    plays_per_game = data_smooth[:,2] / 240
    passes_per_game = data_smooth[:,3] / 240
    passes_per_move = data_smooth[:,3] / (data_smooth[:,2] + data_smooth[:,3])
    moves_per_second = (data_smooth[:,2] + data_smooth[:,3]) / data_smooth[:,1]

    plt.subplot(2, 1, 1)
    # plt.stackplot(data[:,0], data[:,3], data[:,2], colors=('red', 'yellow'))
    # plt.stackplot(data[:,0], csd[:,3], csd[:,2], colors=('red', 'yellow'))
    plt.stackplot(x_smooth, passes_per_game, plays_per_game, colors=('red', 'yellow'))
    # plt.set_ylabel('count')
    plt.twinx()
    plt.plot(x_smooth, passes_per_move)
    # plt.set_ylabel('% pass')
    plt.xlim((0, 1000))

    plt.subplot(2, 1, 2)
    # y = 240 * data[:,0] / csd[:,1]
    plt.plot(moves_per_second)

    plt.show()


if __name__ == '__main__':
    main()
