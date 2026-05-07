#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import subprocess

def main():
    parser = argparse.ArgumentParser(description="Tool for running chess engine matches")
    parser.add_argument("engine1", help="Name of the first engine")
    parser.add_argument("-g", "--games", type=int, default=20, help="Number of games")
    parser.add_argument("-t", "--time", type=str, default="5+0.1", help="Time control")
    args = parser.parse_args()

    engine1 = args.engine1
    games = args.games
    time = args.time

    script_dir = Path(__file__).parent.absolute()
    results_dir = script_dir / "results"
    results_dir.mkdir(parents=True, exist_ok=True)
    results_file = results_dir / f"{engine1}_gauntlet.pgn"
    engines_dir = script_dir / "engines"

    command = [
        "./tools/fastchess",
        "-engine", f"cmd=./engines/{engine1}", f"name={engine1}",
    ]

    engines = [p.name for p in engines_dir.glob("*") if p.name != engine1]
    print("Opponents:\n" + "\n".join(engines))

    for eng in engines:
        command.append("-engine")
        command.append(f"cmd=./engines/{eng}")
        command.append(f"name={eng}")

    command += [
        "-tournament", "gauntlet",
        "-rounds", str(games), "-repeat",
        "-each", f"tc={time}",
        "-openings", "file=UHO_2024_8mvs_+090_+099.pgn", "format=pgn", "order=random",
        "-concurrency", str(8),
        "-recover",
        "-pgnout", f"file={results_file}",
    ]

    subprocess.run(command)
    
    files = [str(p) for p in results_dir.glob("*_gauntlet.pgn")]
    leaderboard = results_dir / "leaderboard.txt"

    subprocess.run([
        "./tools/ordo",
        "-a", "0", "-A", "v1_base", "-Q",
        "-o", str(leaderboard),
        "--aliases=aliases.txt",
        "--groups=results/groups.txt",
        "--"
    ] + files)
    
    print("\n".join(leaderboard.read_text().splitlines()[:20]))

if __name__ == "__main__":
    main()