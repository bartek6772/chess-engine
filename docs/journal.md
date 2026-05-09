# Journal

Here can be found some interseting observations and records about speed improvement.

## Optimizing perft test

Time and what was changed:

| Time [ms] | What was chnaged                                                        |
| --------- | ----------------------------------------------------------------------- |
| 27 226    | original, including magic bitbords                                      |
| 21 615    | new isSquareAttacked                                                    |
| 16 909    | change move list from vector to array                                   |
| 16 600    | optimize pawn moves generation                                          |
| 17 279    | * after going from PieceList to bitboard scanning (temporal decline)    |
| 16 233    | stop updating PieceLists when making and unmaking moves                 |
| 1 938     | switch to relase mode                                                   |
| 2 522     | * with PieceLists back in relase mode (prove that PieceLists were slow) |
| 1 862     | use array to store move history                                         |

## Versions History

### v1.0 - baseline

First version supporting UCI protocol.

---

### v1.1 - move ordering

New fetaures:

- piece-square-tables
- improved time managment
- move ordering

---

### v1.2 - repetitions

New fetaures:

- zobrist hashing
- repetition detection
- 50 moves draw detection

Bugfixes:

- search not discarding results when interrupted

Nodes per second: 2 604 742

---

### v1.3 - panic move

New fetaures:

- search function always can return first sorted move
- new time management logic

---

### v2 - transposition table

Reduced number of swaps and returning best evaluation value instead of alpha in search function.

| Nodes at depth 8 | Version                                                                                          |
| ---------------- | ------------------------------------------------------------------------------------------------ |
| 16 317 252       | v1.3                                                                                             |
| 10 862 274       | Reduced number of swaps and returning best evaluation value instead of alpha in search function. |
| 3 537 846        | Transposition table                                                                              |

New features:

- Transposition tables
- Improved search function

---

### v3 - improved move generation

Nodes per second: 5,273,934

Depth 8 search results:

| Nodes per second | Version                                                    |
| ---------------- | ---------------------------------------------------------- |
| 2 775 302        | v2                                                         |
| 5 273 934        | filtering legal moves after captures in quienscence search |

New features:

- Optimized captures generation

Bugfixes:

- Not storing interrupted search results in transposition table
