#!/usr/bin/env python3
"""Wait for a finished fatigue run, save the CSVs, and refresh the plots.

The Opta sends two blocks over Serial when a run finishes (the START press
that turns the machine off):

    ANGLE_CSV_BEGIN ... ANGLE_CSV_END   -> raw angle samples over time
    STROKE_CSV_BEGIN ... STROKE_CSV_END -> one row per left/right stroke

This script writes:

    visualizations/steering angle/random steering test.csv  (+ .png)
    visualizations/steering angle/stroke endpoints.csv       (+ .png)
"""

import glob
import os
import time

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import serial

BAUD = 115200

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, "visualizations", "steering angle")

ANGLE_CSV_PATH = os.path.join(OUT_DIR, "random steering test.csv")
ANGLE_PNG_PATH = os.path.join(OUT_DIR, "random steering test.png")

STROKE_CSV_PATH = os.path.join(OUT_DIR, "stroke endpoints.csv")
STROKE_PNG_PATH = os.path.join(OUT_DIR, "stroke endpoints.png")


def find_port():
    matches = sorted(glob.glob("/dev/cu.usbmodem*"))
    return matches[0] if matches else None


def open_port():
    while True:
        port = find_port()
        if port is None:
            print("Waiting for the Opta...")
            time.sleep(2)
            continue
        try:
            link = serial.Serial(port, BAUD, timeout=0.5)
            print("Listening on", port)
            return link
        except serial.SerialException as err:
            print("Port busy, retrying:", err)
            time.sleep(2)


def save_angle_plot(times, angles):
    fig, ax = plt.subplots(figsize=(11, 4.4), dpi=160)
    fig.patch.set_facecolor("#f6f4ef")
    ax.set_facecolor("#f6f4ef")

    ax.axhline(0, color="#b7b1a6", linewidth=1, zorder=0)
    ax.plot(times, angles, color="#1d4e89", linewidth=1.8, solid_capstyle="round")
    ax.fill_between(times, angles, 0, color="#1d4e89", alpha=0.12, linewidth=0)

    ax.set_title("Steering angle over time", loc="left", fontsize=16, color="#1c1a17", pad=12)
    ax.set_xlabel("Time (s)", color="#3d3a34")
    ax.set_ylabel("Steering angle (deg)", color="#3d3a34")
    ax.tick_params(colors="#3d3a34")
    ax.grid(True, color="#e2ddd4", linewidth=0.8)
    for spine in ax.spines.values():
        spine.set_color("#d5d0c6")

    fig.tight_layout()
    fig.savefig(ANGLE_PNG_PATH)
    plt.close(fig)


def write_angle_run(rows):
    os.makedirs(OUT_DIR, exist_ok=True)

    with open(ANGLE_CSV_PATH, "w", encoding="utf-8") as handle:
        handle.write("t_s,angle_deg\n")
        for t_s, angle in rows:
            handle.write("%.3f,%.2f\n" % (t_s, angle))

    if len(rows) < 2:
        print("Angle CSV saved with %d row, plot needs at least 2" % len(rows))
        return

    times = [row[0] for row in rows]
    angles = [row[1] for row in rows]
    save_angle_plot(times, angles)
    print("Saved", ANGLE_CSV_PATH)
    print("Saved", ANGLE_PNG_PATH)


def save_stroke_plot(rows):
    left = [(cycle, endpoint) for cycle, direction, endpoint, _, _ in rows if direction == "L"]
    right = [(cycle, endpoint) for cycle, direction, endpoint, _, _ in rows if direction == "R"]

    fig, ax = plt.subplots(figsize=(11, 4.4), dpi=160)
    fig.patch.set_facecolor("#f6f4ef")
    ax.set_facecolor("#f6f4ef")

    ax.axhline(0, color="#b7b1a6", linewidth=1, zorder=0)

    if left:
        ax.plot(
            [c for c, _ in left],
            [e for _, e in left],
            color="#c0392b",
            marker="o",
            markersize=3,
            linewidth=1.4,
            label="Left endpoint",
        )

    if right:
        ax.plot(
            [c for c, _ in right],
            [e for _, e in right],
            color="#1d4e89",
            marker="o",
            markersize=3,
            linewidth=1.4,
            label="Right endpoint",
        )

    ax.set_title("Stroke endpoint angle per cycle", loc="left", fontsize=16, color="#1c1a17", pad=12)
    ax.set_xlabel("Cycle", color="#3d3a34")
    ax.set_ylabel("Endpoint angle (deg)", color="#3d3a34")
    ax.tick_params(colors="#3d3a34")
    ax.grid(True, color="#e2ddd4", linewidth=0.8)
    ax.legend(loc="best", frameon=False)
    for spine in ax.spines.values():
        spine.set_color("#d5d0c6")

    fig.tight_layout()
    fig.savefig(STROKE_PNG_PATH)
    plt.close(fig)


def write_stroke_run(rows):
    os.makedirs(OUT_DIR, exist_ok=True)

    with open(STROKE_CSV_PATH, "w", encoding="utf-8") as handle:
        handle.write("cycle,direction,endpoint_deg,average_rpm,moving_time_ms\n")
        for cycle, direction, endpoint, rpm, moving_ms in rows:
            handle.write("%d,%s,%.2f,%.2f,%d\n" % (cycle, direction, endpoint, rpm, moving_ms))

    if not rows:
        print("Stroke CSV saved with 0 rows, nothing to plot")
        return

    save_stroke_plot(rows)
    print("Saved", STROKE_CSV_PATH)
    print("Saved", STROKE_PNG_PATH)


def main():
    link = open_port()

    capturing_angle = False
    angle_rows = []

    capturing_stroke = False
    stroke_rows = []

    while True:
        try:
            raw = link.readline()
        except serial.SerialException:
            print("Port closed. Reconnecting...")
            try:
                link.close()
            except serial.SerialException:
                pass
            link = open_port()
            capturing_angle = False
            angle_rows = []
            capturing_stroke = False
            stroke_rows = []
            continue

        if not raw:
            continue

        text = raw.decode("utf-8", errors="ignore").strip()

        if text == "ANGLE_CSV_BEGIN":
            capturing_angle = True
            angle_rows = []
            print("Receiving angle log...")
            continue

        if text == "ANGLE_CSV_END":
            capturing_angle = False
            write_angle_run(angle_rows)
            angle_rows = []
            continue

        if text == "STROKE_CSV_BEGIN":
            capturing_stroke = True
            stroke_rows = []
            print("Receiving stroke log...")
            continue

        if text == "STROKE_CSV_END":
            capturing_stroke = False
            write_stroke_run(stroke_rows)
            stroke_rows = []
            continue

        if capturing_angle:
            if text.startswith("t_s"):
                continue
            parts = text.split(",")
            if len(parts) != 2:
                continue
            try:
                angle_rows.append((float(parts[0]), float(parts[1])))
            except ValueError:
                continue
            continue

        if capturing_stroke:
            if text.startswith("cycle"):
                continue
            parts = text.split(",")
            if len(parts) != 5:
                continue
            try:
                stroke_rows.append(
                    (
                        int(parts[0]),
                        parts[1],
                        float(parts[2]),
                        float(parts[3]),
                        int(parts[4]),
                    )
                )
            except ValueError:
                continue
            continue


if __name__ == "__main__":
    main()
