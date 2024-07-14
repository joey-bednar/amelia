# Annie Chess Engine

## About

Annie is a [UCI](https://www.chessprogramming.org/UCI) chess engine written
from scratch in C. You can play Annie on [Lichess.org](https://lichess.org/@/AnnieEdison) or
play locally using a chess interface like [SCID vs PC](https://scidvspc.sourceforge.net/).

## Lichess

Challenge Annie on Lichess under the username [AnnieEdison](https://lichess.org/@/AnnieEdison).
You can also [spectate active games](https://lichess.org/@/AnnieEdison/tv).

## Technical Features

- [Bitboards](https://www.chessprogramming.org/Bitboards)
- [Alpha-beta pruning](https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning)
- [Zobrist hashing](https://en.wikipedia.org/wiki/Zobrist_hashing)
- [Quiescence search](https://en.wikipedia.org/wiki/Quiescence_search)
- [Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening)
- [Triangular PV-table](https://www.chessprogramming.org/Triangular_PV-Table)
- Move Ordering ([Principal variation](https://www.chessprogramming.org/Principal_Variation), all captures)
- [Check Extensions](https://www.chessprogramming.org/Check_Extensions)
- [Null Move Pruning](https://en.wikipedia.org/wiki/Null-move_heuristic)
- [Piece square tables](https://www.chessprogramming.org/Piece-Square_Tables)
- [Negamax](https://en.wikipedia.org/wiki/Negamax) search
- [Mop-up Evaluation](https://www.chessprogramming.org/Mop-up_Evaluation)
- Bulk [perft](https://www.chessprogramming.org/Perft) testing
- [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) compatible

## Build

```bash
git clone https://github.com/joey-bednar/annie && \
cd annie && \
make && \
./build/annie
```
