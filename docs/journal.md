# Journal

Here can be found some interseting observations and records about speed improvement.

## Optimizing perft test

Time and what was changed:

| Time [ms] | What was chnaged |
| --- | --- |
| 27 226 | original, including magic bitbords |
| 21 615 | new isSquareAttacked |
| 16 909 | change move list from vector to array |
| 16 600 | optimize pawn moves generation |
| 17 279 | after going from PieceList to bitboard scanning |
| 16 233 | stop updating PieceLists when making and unmaking moves |
| 1 938 | switch to relase mode |
| 2 522 | with PieceLists back in relase mode |
| 1 862 | use array to store move history |

## Versions History

### v1.0 - baseline

First version supporting UCI protocol

### v1.1 - move ordering

New fetaures:

- piece-square-tables
- improved time managment
- move ordering

### v1.2 - repetitions

Nodes per second: 2 604 742

New fetaures:

- zobrist hashing
- repetition detection
- 50 moves draw detection

### v1.2.1 - timeouts fix

New fetaures:

- playing first avaliable move

### v1.2.2 - better timeouts fix

New fetaures:

- playing first sorted move from search function
