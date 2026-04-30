"""
Comparison plots: Proof Size, Proving Time, Verifying Time
across ASC-CRS, ASC-SRS, DualDory, and DID:RING for group sizes where
all schemes have data points.
"""

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker

# ---------------------------------------------------------------------------
# Load
# ---------------------------------------------------------------------------
asc_crs_raw = pd.read_csv("asc_crs_data.csv", skipinitialspace=True)
asc_srs_raw = pd.read_csv("asc_srs_data.csv", skipinitialspace=True)
dualdory_raw = pd.read_csv("dualdory_data.csv", skipinitialspace=True)
didring_raw = pd.read_csv("didring_data.csv", skipinitialspace=True)

# ---------------------------------------------------------------------------
# Normalise ASC-CRS
#   - keep only single-verifier rows (verifier_number is NaN)
#   - convert times s → ms
# ---------------------------------------------------------------------------
asc_crs = asc_crs_raw[asc_crs_raw["verifier_number"].isna()].copy()
asc_crs = asc_crs.rename(columns={
    "proof_size":        "proof_bytes",
    "proving_time(s)":   "proving_ms",
    "verifying_time(s)": "verifying_ms",
})
asc_crs["proving_ms"]   *= 1000
asc_crs["verifying_ms"] *= 1000

# ASC-CRS rows for L=100 (stored in verifier_number column).
asc_crs_l100 = asc_crs_raw[asc_crs_raw["verifier_number"] == 100].copy()
asc_crs_l100 = asc_crs_l100.rename(columns={
    "proof_size":        "proof_bytes",
    "proving_time(s)":   "proving_ms",
    "verifying_time(s)": "verifying_ms",
})
asc_crs_l100["proving_ms"] *= 1000
asc_crs_l100["verifying_ms"] *= 1000

# ---------------------------------------------------------------------------
# Normalise ASC-SRS (already in ms)
# ---------------------------------------------------------------------------
asc_srs = asc_srs_raw.rename(columns={
    "proof_size(B)":       "proof_bytes",
    "proving_time(ms)":    "proving_ms",
    "verifying_time(ms)":  "verifying_ms",
})

# ---------------------------------------------------------------------------
# Normalise DualDory (already in ms; column called signing_time)
# ---------------------------------------------------------------------------
dualdory = dualdory_raw.rename(columns={
    "proof_size(B)":      "proof_bytes",
    "signing_time(ms)":   "proving_ms",
    "verifying_time(ms)": "verifying_ms",
})

# ---------------------------------------------------------------------------
# Normalise DID:RING (already in ms; column called signing_time)
# ---------------------------------------------------------------------------
didring = didring_raw.rename(columns={
    "proof_size(B)":      "proof_bytes",
    "signing_time(ms)":   "proving_ms",
    "verifying_time(ms)": "verifying_ms",
})

# ---------------------------------------------------------------------------
# Find common group sizes
# ---------------------------------------------------------------------------
common_gs = sorted(
    set(asc_crs["group_size"])
    & set(asc_srs["group_size"])
    & set(dualdory["group_size"])
    & set(didring["group_size"])
)
common_gs_l100 = sorted(
    set(asc_crs_l100["group_size"])
    & set(asc_srs["group_size"])
    & set(dualdory["group_size"])
    & set(didring["group_size"])
)
print("Common group sizes (single-verifier ASC-CRS):", common_gs)
print("Common group sizes (ASC-CRS L=100):", common_gs_l100)

df_crs = (asc_crs[asc_crs["group_size"].isin(common_gs)]
          .sort_values("group_size").reset_index(drop=True))
df_crs_l100 = (asc_crs_l100[asc_crs_l100["group_size"].isin(common_gs_l100)]
               .sort_values("group_size").reset_index(drop=True))
df_srs = (asc_srs[asc_srs["group_size"].isin(common_gs)]
          .sort_values("group_size").reset_index(drop=True))
df_dd  = (dualdory[dualdory["group_size"].isin(common_gs)]
          .sort_values("group_size").reset_index(drop=True))
df_did = (didring[didring["group_size"].isin(common_gs)]
          .sort_values("group_size").reset_index(drop=True))

df_srs_l100 = (asc_srs[asc_srs["group_size"].isin(common_gs_l100)]
               .sort_values("group_size").reset_index(drop=True))
df_dd_l100  = (dualdory[dualdory["group_size"].isin(common_gs_l100)]
               .sort_values("group_size").reset_index(drop=True))
df_did_l100 = (didring[didring["group_size"].isin(common_gs_l100)]
               .sort_values("group_size").reset_index(drop=True))

# ---------------------------------------------------------------------------
# Plot
# ---------------------------------------------------------------------------
SCHEMES_DEFAULT = [
    ("ASC-CRS",  df_crs, "tab:blue",   "o"),
    ("ASC-SRS",  df_srs, "tab:orange", "s"),
    ("DualDory", df_dd,  "tab:green",  "^"),
    ("DID:RING", df_did, "tab:red",    "D"),
]

SCHEMES_L100 = [
    ("ASC-CRS (L=100)", df_crs_l100, "tab:blue", "o"),
    ("ASC-SRS",         df_srs_l100, "tab:orange", "s"),
    ("DualDory",        df_dd_l100,  "tab:green",  "^"),
    ("DID:RING",        df_did_l100, "tab:red",    "D"),
]

METRICS = [
    ("proof_bytes",  "Proof Size (bytes)",   "Proof Size Comparison"),
    ("proving_ms",   "Proving Time (ms)",    "Proving Time Comparison"),
    ("verifying_ms", "Verifying Time (ms)",  "Verifying Time Comparison"),
]

for col, ylabel, title in METRICS:
    fig, ax = plt.subplots(figsize=(7, 5))
    schemes = SCHEMES_L100 if col in {"proof_bytes", "proving_ms"} else SCHEMES_DEFAULT
    gs_ticks = common_gs_l100 if col in {"proof_bytes", "proving_ms"} else common_gs
    for label, df, color, marker in schemes:
        y_values = df[col] / 1024.0 if col == "proof_bytes" else df[col]
        ax.plot(
            df["group_size"], y_values,
            marker=marker, color=color, label=label,
            linewidth=1.8, markersize=7,
        )
    ax.set_xscale("log", base=2)
    ax.set_xticks(gs_ticks)
    ax.get_xaxis().set_major_formatter(ticker.ScalarFormatter())
    ax.set_xlabel("Group Size N", fontsize=16)
    ax.set_ylabel("Proof Size (KB)" if col == "proof_bytes" else ylabel, fontsize=16)
    ax.set_title(title, fontsize=18)
    ax.legend(fontsize=14, loc="upper left", framealpha=1.0)
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.tick_params(axis="x", rotation=45, labelsize=14)
    ax.tick_params(axis="y", labelsize=14)
    plt.tight_layout()
    name_map = {"proof_bytes": "proof_size_n", "proving_ms": "proof_proving_n", "verifying_ms": "proof_verification_n"}
    out_path = f"{name_map[col]}.pdf"
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    print(f"Saved {out_path}")
    plt.close(fig)

# ---------------------------------------------------------------------------
# ASC-CRS only (multi-verifier rows)
# ---------------------------------------------------------------------------
asc_crs_mv = asc_crs_raw[asc_crs_raw["verifier_number"].notna()].copy()
asc_crs_mv = asc_crs_mv.rename(columns={
    "proof_size":        "proof_bytes",
    "proving_time(s)":   "proving_ms",
    "verifying_time(s)": "verifying_ms",
})
asc_crs_mv["proving_ms"] *= 1000
asc_crs_mv["verifying_ms"] *= 1000
asc_crs_mv["verifier_number"] = asc_crs_mv["verifier_number"].astype(int)

mv_group_sizes = sorted(asc_crs_mv["group_size"].unique())
mv_verifiers = sorted(asc_crs_mv["verifier_number"].unique())

asc_crs_mv_pow2 = asc_crs_mv[asc_crs_mv["group_size"].apply(lambda n: n > 0 and (int(n) & (int(n) - 1)) == 0)].copy()
mv_group_sizes_pow2 = sorted(asc_crs_mv_pow2["group_size"].unique())

COLORS = ["tab:blue", "tab:orange", "tab:green", "tab:red", "tab:brown", "tab:pink"]
MARKERS = ["o", "s", "^", "D", "v", "P"]

def plot_metric_by_group_for_each_verifier(metric_col, ylabel, title, out_path, source_df, x_ticks):
    fig, ax = plt.subplots(figsize=(8, 5))
    for idx, verifier in enumerate(mv_verifiers):
        subset = source_df[source_df["verifier_number"] == verifier].sort_values("group_size")
        ax.plot(
            subset["group_size"],
            subset[metric_col],
            label=f"L={verifier}",
            color=COLORS[idx % len(COLORS)],
            marker=MARKERS[idx % len(MARKERS)],
            linewidth=1.8,
            markersize=6,
        )
    ax.set_xscale("log", base=2)
    ax.set_xticks(x_ticks)
    ax.get_xaxis().set_major_formatter(ticker.ScalarFormatter())
    ax.set_xlabel("Group Size N", fontsize=16)
    ax.set_ylabel(ylabel, fontsize=16)
    ax.set_title(title, fontsize=18)
    ax.legend(fontsize=14, loc="upper left", framealpha=1.0)
    ax.grid(True, which="both", linestyle="--", alpha=0.4)
    ax.tick_params(axis="x", rotation=45, labelsize=14)
    ax.tick_params(axis="y", labelsize=14)
    plt.tight_layout()
    plt.savefig(out_path, dpi=150, bbox_inches="tight")
    print(f"Saved {out_path}")
    plt.close(fig)

plot_metric_by_group_for_each_verifier(
    "proving_ms",
    "Proving Time (ms)",
    "ASC-CRS Proving Time by Group Size (one line per verifier count)",
    "proving_L.pdf",
    asc_crs_mv_pow2,
    mv_group_sizes_pow2,
)

plot_metric_by_group_for_each_verifier(
    "verifying_ms",
    "Verifying Time (ms)",
    "ASC-CRS Verifying Time by Group Size (one line per verifier count)",
    "verification_L.pdf",
    asc_crs_mv,
    mv_group_sizes,
)

# Summary over verifier_number with fixed N=1024.
asc_crs_mv_n1024 = asc_crs_mv[asc_crs_mv["group_size"] == 1024].copy()

# One point per verifier count for each metric at N=1024.
asc_crs_by_verifier = (
    asc_crs_mv_n1024.groupby("verifier_number", as_index=False)[["proof_bytes", "proving_ms", "verifying_ms"]]
    .mean()
    .sort_values("verifier_number")
)
asc_crs_by_verifier["proof_kb"] = asc_crs_by_verifier["proof_bytes"] / 1024.0

fig, ax1 = plt.subplots(figsize=(8, 5))
ax2 = ax1.twinx()
ax1.set_zorder(ax2.get_zorder() + 1)
ax1.patch.set_visible(False)

line1 = ax1.plot(
    asc_crs_by_verifier["verifier_number"],
    asc_crs_by_verifier["proof_kb"],
    marker="o",
    color="tab:blue",
    linewidth=1.8,
    label="Proof Size (KB)",
)
line2 = ax2.plot(
    asc_crs_by_verifier["verifier_number"],
    asc_crs_by_verifier["proving_ms"],
    marker="s",
    color="tab:orange",
    linewidth=1.8,
    label="Proving Time (ms)",
)
line3 = ax2.plot(
    asc_crs_by_verifier["verifier_number"],
    asc_crs_by_verifier["verifying_ms"],
    marker="^",
    color="tab:green",
    linewidth=1.8,
    label="Verifying Time (ms)",
)

ax1.set_xlabel("L", fontsize=16)
ax1.set_ylabel("Proof Size (KB)", color="tab:blue", fontsize=16)
ax2.set_ylabel("Time (ms)", fontsize=16)
ax1.set_title("ASC-CRS Metrics vs L (N=1024)", fontsize=18)
ax1.set_xscale("log")
ax1.set_xticks(asc_crs_by_verifier["verifier_number"].tolist())
ax1.get_xaxis().set_major_formatter(ticker.ScalarFormatter())
ax1.tick_params(axis="x", rotation=45, labelsize=14)
ax1.tick_params(axis="y", labelsize=14)
ax2.tick_params(axis="y", labelsize=14)
ax1.grid(True, linestyle="--", alpha=0.4)

all_lines = line1 + line2 + line3
all_labels = [line.get_label() for line in all_lines]
leg = ax1.legend(all_lines, all_labels, fontsize=14, loc="upper left", framealpha=1.0)
leg.set_zorder(10)

plt.tight_layout()
out_path = "agg_L.pdf"
plt.savefig(out_path, dpi=150, bbox_inches="tight")
print(f"Saved {out_path}")
plt.close(fig)
