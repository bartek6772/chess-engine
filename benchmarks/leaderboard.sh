#!/bin/bash

OFFICIAL_FILES=$(find results -name "*.pgn" ! -name "tmp.pgn")
    
if [ -n "$OFFICIAL_FILES" ]; then
    ./tools/ordo -a 0 -A "v1_fixed" -Q -o results/leaderboard.txt -- $OFFICIAL_FILES
    echo "Leaderboard updated."
    head results/leaderboard.txt -n 10
else
    echo "No official PGN files found."
fi