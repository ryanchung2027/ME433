import csv
import numpy as np


def load_csv(filename):   
    time = [] # column 0
    data = [] # column 1
    with open(filename, 'r', newline='') as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            if len(row) < 2:
                continue
            try:
                t = float(row[0])
                v = float(row[1])
            except ValueError:
                continue
            # read the rows 1 one by one
            time.append(float(row[0])) # leftmost column
            data.append(float(row[1])) # second column
    return np.array(time), np.array(data)


# Quick check: load sigA.csv and print basic info
tA, xA = load_csv('sigA.csv')
print(f"sigA.csv → {len(tA)} samples; first time={tA[0]:.6f}, last time={tA[-1]:.6f}")