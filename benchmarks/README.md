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

- `./build_current.sh <current_name>` - builds the engine and copies its executable
- `./build.sh <git_tag>` - builds previous version of the engine
- `./run.sh <engine_1> <engine_2> [--unofficial]` - run SPRT test between two versions; flag determines if those games should affect the leaderboard; should be executed from it's directory
