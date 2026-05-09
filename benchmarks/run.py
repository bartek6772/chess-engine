#!/usr/bin/env python3

import argparse
import subprocess

def main():
    parser = argparse.ArgumentParser(description="Tool for running chess engine matches")

    parser.add_argument("engine1", help="Name of the first engine")
    parser.add_argument("engine2", help="Name of the second engine")
    parser.add_argument("-g", "--games", type=int, default=20, help="Number of games")
    parser.add_argument("-t", "--time", type=str, default="5+0.1", help="Time control")
    parser.add_argument("-r", "--regression", action="store_true", help="Do regression test")

    args = parser.parse_args()

    engine1 = args.engine1
    engine2 = args.engine2
    games = args.games
    time = args.time

    hypothesis = (0, 40)
    if args.regression:
        hypothesis = (-40, 0)


    subprocess.run([
        "./tools/fastchess",
        "-engine", f"cmd=./engines/{engine1}", f"name={engine1}",
        "-engine", f"cmd=./engines/{engine2}", f"name={engine2}",
        "-openings", "file=UHO_2024_8mvs_+090_+099.pgn", "format=pgn", "order=random",
        "-concurrency", str(8),
        "-recover",
        "-each", f"tc={time}",
        "-rounds", str(games), "-repeat",
        "-sprt", f"elo0={hypothesis[0]}", f"elo1={hypothesis[1]}", "alpha=0.1", "beta=0.1",
    ])

if __name__ == "__main__":
    main()