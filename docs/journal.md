# Journal
Here can be found interseting observations and records speed improvement.

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
Version:
- v1.0.0 - first version supporting UCI protocol
- v1.1.0 - add piece-square-tables and improve time management