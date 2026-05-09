#!/usr/bin/env python3

import argparse
import os
from pathlib import Path
import subprocess

def version_key(name: str):
    version = name.split("_")[0][1:]
    return [int(num) for num in version.split(".")]

def main():
    parser = argparse.ArgumentParser(description="Tool for running chess engine matches")
    parser.add_argument("engine1", help="Name of the first engine")
    parser.add_argument("-g", "--games", type=int, default=20, help="Number of games")
    parser.add_argument("-t", "--time", type=str, default="5+0.1", help="Time control")
    parser.add_argument("-o", "--opponents", type=int, default=3, help="Play against number of latest versions")
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

    engines = [p.name for p in engines_dir.glob("*")]

    if engines.count(engine1) == 0:
        print(f"Engine {engine1} isn't present in {engines_dir}")
        return

    engines.sort(key=version_key)
    engine_idx = engines.index(engine1)

    opponents = engines[max(0, engine_idx - args.opponents):engine_idx]
    print("Opponents:\n" + "\n".join(opponents))

    for eng in opponents:
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
    
    files = [str(p) for p in results_dir.glob("v*_gauntlet.pgn")]
    leaderboard = results_dir / "leaderboard.txt"

    subprocess.run([
        "./tools/ordo",
        "-a", "0", "-A", "v1_base", "-Q",
        "-o", str(leaderboard),
        "--aliases=aliases.txt",
        "--groups=results/groups.txt",
        "-j", "results/h2h.txt",
        "--"
    ] + files)
    
    print("\n".join(leaderboard.read_text().splitlines()[:20]))

if __name__ == "__main__":
    main()