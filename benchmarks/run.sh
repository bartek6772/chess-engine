#!/bin/bash

if [[ -z "$1" || -z "$2" ]]; then
    echo "Usage ./run.sh <engine_1> <engine_2> [--unofficial]"
    exit 1
fi

engine1=$1
engine2=$2
PGN_OUTPUT="results/${engine1}_vs_${engine2}.pgn"
IS_OFFICIAL=true

for arg in "$@"; do
    if [ "$arg" == "--unofficial" ]; then
        PGN_OUTPUT="results/tmp.pgn"
        IS_OFFICIAL=false
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

if [ "$IS_OFFICIAL" = true ]; then
    echo "Re-calculating leaderboard from all official records..."
    
    # Filter for PGNs that are NOT tmp.pgn
    OFFICIAL_FILES=$(find results -name "*.pgn" ! -name "tmp.pgn")
    
    if [ -n "$OFFICIAL_FILES" ]; then
        ./tools/ordo -a 0 -A "v1_fixed" -Q -o results/leaderboard.txt -- $OFFICIAL_FILES
        echo "Leaderboard updated."
        head results/leaderboard.txt -n 10
    else
        echo "No official PGN files found."
    fi
else
    echo "Test run complete. Leaderboard was not updated."
fi