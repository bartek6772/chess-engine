#!/bin/bash

if [[ -z "$1" || -z "$2" ]]; then
    echo "Usage ./run.sh <engine_1> <engine_2> [--official]"
    exit 1
fi

engine1=$1
engine2=$2
PGN_OUTPUT="results/tmp.pgn"

for arg in "$@"; do
    if [ "$arg" == "--official" ]; then
        PGN_OUTPUT="results/${engine1}_vs_${engine2}.pgn"
    fi
done

mkdir -p results

./tools/fastchess \
-engine cmd="./engines/$engine1" name="$engine1" \
-engine cmd="./engines/$engine2" name="$engine2" \
-openings file=8moves_v3.pgn format=pgn order=random \
-each tc=8+0.08 \
-rounds 20 -repeat \
-concurrency 8 \
-recover \
-sprt elo0=0 elo1=10 alpha=0.05 beta=0.1 \
-pgnout file="$PGN_OUTPUT" \

# Update leader board
# ./tools/ordo -p results/games.pgn -a 0 -A "v1_fixed" -Q -o results/leaderboard.txt