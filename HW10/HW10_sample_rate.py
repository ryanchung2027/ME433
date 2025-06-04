from HW10_read_csv import load_csv

def compute_sample_rate(times):
    N = len(times)
    t_total = times[-1] - times[0]
    if t_total <= 0:
        raise ValueError("Time vector must strictly increase.")
    return N / t_total

if __name__ == '__main__':
    for fname in ['sigA.csv', 'sigB.csv', 'sigC.csv', 'sigD.csv']:
        t, x = load_csv(fname)
        sr = compute_sample_rate(t)
        print(f"{fname}: N={len(t)}, t_start={t[0]:.6f} s, "
              f"t_end={t[-1]:.6f} s → SR ≈ {sr:.2f} Hz")