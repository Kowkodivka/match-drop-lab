# /// script
# requires-python = ">=3.12"
# dependencies = [
#     "matplotlib>=3.8",
#     "numpy>=1.26",
#     "scipy>=1.11",
# ]
# ///

from __future__ import annotations

import math
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

L = 67.0
K_STUDENT = 2.0
INSTR_ERR = 0.5
INSTR_ERR_L = 0.5
SEED = 42
TRIALS = [30, 50, 100, 500, 1000, 5000, 10000]
DATA_FILE = Path("data.csv")
FIGURES_DIR = Path("figures")


def load_data(path: Path) -> tuple[np.ndarray, np.ndarray]:
    data = np.loadtxt(path, delimiter=",", skiprows=1)
    return data[:, 0], data[:, 1]


def random_error(values: np.ndarray, mean: float, k: float = K_STUDENT) -> float:
    n = len(values)
    sum_sq = np.sum((values - mean) ** 2)
    return k * math.sqrt(sum_sq / (n * (n - 1)))


def full_error(rand_err: float, instr_err: float = INSTR_ERR) -> float:
    return math.sqrt(rand_err**2 + instr_err**2)


def indirect_error(
    Q: float, L_val: float, dL: float, mean_val: float, d_mean: float
) -> float:
    rel_L = dL / L_val
    rel_mean = d_mean / mean_val
    return Q * math.sqrt(rel_L**2 + rel_mean**2)


def process_experiment(lx: np.ndarray, ly: np.ndarray) -> None:
    n = len(lx)
    mean_x = float(np.mean(lx))
    mean_y = float(np.mean(ly))

    rand_x = random_error(lx, mean_x)
    rand_y = random_error(ly, mean_y)
    full_x = full_error(rand_x)
    full_y = full_error(rand_y)

    A = L / mean_x
    B = L / mean_y
    err_A = indirect_error(A, L, INSTR_ERR_L, mean_x, full_x)
    err_B = indirect_error(B, L, INSTR_ERR_L, mean_y, full_y)

    print(f"Measurements: {n}")
    print(f"mean lx = {mean_x:.4f} mm")
    print(f"random error = {rand_x:.4f} mm")
    print(f"total error = {full_x:.4f} mm")
    print(f"mean ly = {mean_y:.4f} mm")
    print(f"random error = {rand_y:.4f} mm")
    print(f"total error = {full_y:.4f} mm")
    print(f"L = {L:.4f} +- {INSTR_ERR_L:.4f} mm")
    print(f"A = {A:.4f} +- {err_A:.4f}")
    print(f"B = {B:.4f} +- {err_B:.4f}")
    print()


def simulate(n: int, rng: np.random.Generator) -> dict:
    theta = rng.uniform(0.0, 2.0 * np.pi, size=n)
    lx = L * np.abs(np.cos(theta))
    ly = L * np.abs(np.sin(theta))

    mean_x = float(np.mean(lx))
    mean_y = float(np.mean(ly))
    rand_x = random_error(lx, mean_x)
    rand_y = random_error(ly, mean_y)
    A = L / mean_x
    B = L / mean_y

    return {
        "n": n,
        "mean_lx": mean_x,
        "rand_lx": rand_x,
        "mean_ly": mean_y,
        "rand_ly": rand_y,
        "A": A,
        "B": B,
    }


def run_simulations(seed: int = SEED) -> list[dict]:
    rng = np.random.default_rng(seed)
    results = []

    for n in TRIALS:
        r = simulate(n, rng)
        results.append(r)
        print(f"n = {n}")
        print(f"mean lx = {r['mean_lx']:.4f} mm")
        print(f"random error = {r['rand_lx']:.4f} mm")
        print(f"mean ly = {r['mean_ly']:.4f} mm")
        print(f"random error = {r['rand_ly']:.4f} mm")
        print(f"A = {r['A']:.4f}")
        print(f"B = {r['B']:.4f}")
        print(f"theory = {math.pi / 2:.4f}")
        print()

    return results


def plot_convergence(results: list[dict], out_path: Path) -> None:
    ns = [r["n"] for r in results]
    As = [r["A"] for r in results]
    Bs = [r["B"] for r in results]

    fig, ax = plt.subplots(figsize=(7, 4.5), dpi=150)

    ax.semilogx(
        ns,
        As,
        "o-",
        color="#1f77b4",
        label=r"$A$",
        markersize=6,
        linewidth=1.5,
    )
    ax.semilogx(
        ns,
        Bs,
        "s-",
        color="#d62728",
        label=r"$B$",
        markersize=6,
        linewidth=1.5,
    )
    ax.axhline(
        math.pi / 2,
        color="gray",
        linestyle="--",
        linewidth=1.2,
        label=r"Теоретическое значение",
    )

    ax.set_xlabel(r"$n$")
    ax.set_ylabel(r"$A$, $B$")
    ax.set_title(r"Сходимость $A$ и $B$ к теоретическому значению")
    ax.legend(loc="upper right", fontsize=9)
    ax.grid(True, which="both", linestyle=":", alpha=0.6)
    ax.set_ylim(1.30, 2.00)
    ax.set_xlim(20, 2e4)

    plt.tight_layout()
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, format="png", bbox_inches="tight")
    plt.close(fig)
    print(f"plot saved: {out_path}")
    print()


def main() -> None:
    FIGURES_DIR.mkdir(parents=True, exist_ok=True)

    if not DATA_FILE.exists():
        raise FileNotFoundError(f"missing {DATA_FILE}")

    lx, ly = load_data(DATA_FILE)
    process_experiment(lx, ly)

    results = run_simulations(seed=SEED)
    plot_convergence(results, FIGURES_DIR / "convergence.png")

    print("summary")
    print(f"{'n':>6}  {'lx':>8}  {'ly':>8}  {'A':>7}  {'B':>7}")
    for r in results:
        print(
            f"{r['n']:6d}  {r['mean_lx']:8.2f}  {r['mean_ly']:8.2f}  "
            f"{r['A']:7.3f}  {r['B']:7.3f}"
        )


if __name__ == "__main__":
    main()
