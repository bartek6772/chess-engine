#!/bin/bash

GAMES=20
TIME=5+0.1

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
-each tc=$TIME \
-rounds $GAMES -repeat \
-concurrency 8 \
-recover \
-sprt elo0=0 elo1=10 alpha=0.05 beta=0.1 \
-pgnout file="$PGN_OUTPUT"
# -log file=engine_debug.log level=trace engine=true

# Update leader board
if [ "$IS_OFFICIAL" = true ]; then
    echo "Re-calculating leaderboard from all official records..."
    ./leaderboard.sh
else
    echo "Test run complete. Leaderboard was not updated."
fi