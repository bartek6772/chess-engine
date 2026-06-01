# Benchmarking guide

## Directory structure

```
benchmarks
├── engines
├── results
└── tools
```

Description of subdirectories:

- `engines/` - stores executables of engine versions
- `results/` - stores results of games that are used to create a leaderboard
- `tools/` - stores executables of third-party software

File `8moves_v3.pgn` contains openings required for testing.

## Tools

Tools that are required for running tests:

- **fastchess** - runs an SPRT test and generating games data
- **ordo** - generates a leaderboard from saved games

Tools that are optional, but were used for debugging:

- **cutechess** - can be used for debugging as an alternative to the gui
- **stockfish** - to test some versions of my engine

## Scripts

- `./build.py` - tool for building the engine
- `./run.py` - run SPRT test between two versions

Both supprt -h flag for help and more details about usage.

## Commands

Update leaderboard:

```bash
./tools/ordo -a 0 -A "v1_fixed" -Q -o results/leaderboard.txt -- $(find results -name "*.pgn" ! -name "tmp.pgn")
```

```bash
./tools/ordo -a 0 -A "v1_base" -Q -o results/leaderboard.txt --aliases=aliases.txt -- $(find results -name "*_gauntlet.pgn" ! -name "tmp.pgn")
```
