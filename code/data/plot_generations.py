#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt


def smooth_(x, window_len=10, window='hanning'):
    """smooth the data using a window with requested size.

    from: http://scipy.org/Cookbook/SignalSmooth

    This method is based on the convolution of a scaled window with the signal.
    The signal is prepared by introducing reflected copies of the signal
    (with the window size) in both ends so that transient parts are minimized
    in the begining and end part of the output signal.

    input:
        x: the input signal
        window_len: the dimension of the smoothing window
        window: the type of window from 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'
            flat window will produce a moving average smoothing.

    output:
        the smoothed signal

    example:

    import numpy as np
    t = np.linspace(-2,2,0.1)
    x = np.sin(t)+np.random.randn(len(t))*0.1
    y = smooth(x)

    see also:

    numpy.hanning, numpy.hamming, numpy.bartlett, numpy.blackman, numpy.convolve
    scipy.signal.lfilter

    TODO: the window parameter could be the window itself if an array instead of a string
    """

    if x.ndim != 1:
        raise ValueError("smooth only accepts 1 dimension arrays.")

    if x.size < window_len:
        raise ValueError("Input vector needs to be bigger than window size.")

    if window_len < 3:
        return x

    if not window in ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
        raise ValueError("Window is on of 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'")

    s=np.r_[2*x[0]-x[window_len:1:-1], x, 2*x[-1]-x[-1:-window_len:-1]]
    #print(len(s))

    if window == 'flat': #moving average
        w = np.ones(window_len,'d')
    else:
        w = getattr(np, window)(window_len)
    y = np.convolve(w/w.sum(), s, mode='same')
    return y[window_len-1:-window_len+1]


def plot(inp, outp):
    # 0: generation
    # 1: time
    # 2: games
    # 3: plays
    # 4: passes
    data = np.loadtxt(inp, delimiter=',')
    generations = data[:, 0]
    time = data[:, 1]
    games = 992
    plays = data[:, 3]
    passes = data[:, 4]
    moves = plays + passes
    fig = plt.figure()
    ax1 = fig.add_subplot(2, 1, 1)
    ax1.set_xlim([0, data[0, -1]])
    ax1.plot(generations, moves / games, label='moves')
    ax1.plot(generations, plays / games, label='plays')
    ax1.plot(generations, passes / games, label='passes')
    ax2 = fig.add_subplot(2, 1, 2)
    ax2.set_xlim([0, data[0, -1]])
    ax2.plot(generations, passes / moves, label='passes / moves')
    ax2.legend(loc='best')
    fig.savefig(outp)


def smooth(inp, outp):
    data = np.loadtxt(inp, delimiter=',')
    smoothed = np.empty_like(data)
    smoothed[:, 0] = smooth_(data[:, 0], window_len=10000)
    smoothed[:, 1] = smooth_(data[:, 1], window_len=10000)
    smoothed[:, 2] = smooth_(data[:, 2], window_len=10000)
    smoothed[:, 3] = smooth_(data[:, 3], window_len=10000)
    smoothed[:, 4] = smooth_(data[:, 4], window_len=10000)
    np.savetxt(outp, smoothed, delimiter=',')


def usage():
    print("usage: plot.py [plot|smooth] input output")


def main():
    if len(sys.argv) < 4:
        usage()
        sys.exit(1)

    if sys.argv[1] == 'plot':
        plot(*sys.argv[2:4])
    elif sys.argv[1] == 'smooth':
        smooth(*sys.argv[2:4])
    else:
        usage()


if __name__ == '__main__':
    main()
