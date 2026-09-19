# /// script
# requires-python = ">=3.14"
# dependencies = [
#     "numpy>=2.5.3",
#     "pandas>=3.0.5",
# ]
# ///

from pathlib import Path

import numpy as np
import pandas as pd

L = 67.0
DATA_FILE = Path("data.csv")
OUTPUT_FILE = Path("output.csv")


def main() -> None:
    data = np.loadtxt(DATA_FILE, delimiter=",", skiprows=1)
    data = data / L
    data = pd.DataFrame(data, columns=["A", "B"])
    data.to_csv(OUTPUT_FILE, index=False)


if __name__ == "__main__":
    main()
