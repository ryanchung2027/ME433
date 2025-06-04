import matplotlib.pyplot as plt # for plotting
import numpy as np # for sine function
from HW10_read_csv import load_csv

def plot_time_domain(filename, ax):
    """
    Loads 'filename' (CSV) into arrays, then plots time vs. value on ax.
    """
    t, x = load_csv(filename)
    ax.plot(t, x, color='blue', linewidth=1)
    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Amplitude')
    ax.set_title(f"{filename} (time domain)")
    ax.grid(True)
    return t, x  # return arrays in case you want them

if __name__ == '__main__':
    files = ['sigA.csv', 'sigB.csv', 'sigC.csv', 'sigD.csv']
    fig, axes = plt.subplots(2, 2, figsize=(10, 6))
    axes = axes.flatten()

    for i, fname in enumerate(files):
        plot_time_domain(fname, axes[i])

    fig.tight_layout(pad=2.0)
    plt.savefig('plots/all_signals_time.png', dpi=300)
    plt.show()