# Amelia Chess Engine

## About

Amelia is a [UCI](https://www.chessprogramming.org/UCI) compatible chess engine written
from scratch in C. You can play Amelia on [Lichess.org](https://lichess.org/@/AnnieEdison) or
install a chess interface to play locally.

## Lichess

Challenge Amelia on Lichess under the username [AmeliaLatest](https://lichess.org/@/AmeliaLatest).
You can also [spectate active games](https://lichess.org/@/AmeliaLatest/tv).

## Technical Features

- Board
    - [Bitboards](https://www.chessprogramming.org/Bitboards)
    - [Zobrist hashing](https://en.wikipedia.org/wiki/Zobrist_hashing)
- Search
    - [Negamax](https://en.wikipedia.org/wiki/Negamax) search
    - [Alpha-beta pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)
    - [Quiescence search](https://en.wikipedia.org/wiki/Quiescence_search)
    - [Iterative deepening](https://www.chessprogramming.org/Iterative_Deepening)
    - [Triangular PV-table](https://www.chessprogramming.org/Triangular_PV-Table)
    - Move ordering ([principal variation](https://www.chessprogramming.org/Principal_Variation), [MVV-LVA](https://www.chessprogramming.org/MVV-LVA))
    - [Check extensions](https://www.chessprogramming.org/Check_Extensions)
    - [Null move pruning](https://en.wikipedia.org/wiki/Null-move_heuristic)
- Evaluation
    - Opening/endgame [piece square tables](https://www.chessprogramming.org/Piece-Square_Tables)
    - Passed pawn evaluation
    - [Mop-up evaluation](https://www.chessprogramming.org/Mop-up_Evaluation)
- Interface
    - [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) compatible
    - Bulk [perft](https://www.chessprogramming.org/Perft) testing

## Build

```bash
git clone https://github.com/joey-bednar/amelia && \
cd amelia && \
make && \
./build/amelia
```
