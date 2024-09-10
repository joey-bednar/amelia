<div align="center">

# Amelia
#### Chess engine accepting challenges on [Lichess.org](https://lichess.org/@/AmeliaLatest).

[![lichess-rapid](https://lichess-shield.vercel.app/api?username=AmeliaLatest&format=bullet)](https://lichess.org/@/AmeliaLatest/perf/bullet)
[![lichess-rapid](https://lichess-shield.vercel.app/api?username=AmeliaLatest&format=blitz)](https://lichess.org/@/AmeliaLatest/perf/blitz)

<img src="logo.png" alt="logo" width="200px" height="200px"/>

![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

</div>

## :hibiscus: About

Amelia is a [UCI](https://www.chessprogramming.org/UCI) compatible chess engine written
from scratch in C. You can play Amelia on [Lichess.org](https://lichess.org/@/AmeliaLatest) or
install a chess interface to play locally.

## :hibiscus: Lichess

Challenge Amelia on Lichess under the username [AmeliaLatest](https://lichess.org/@/AmeliaLatest).
You can also [spectate active games](https://lichess.org/@/AmeliaLatest/tv).

## :hibiscus: Technical Features

- Board
    - [Bitboards](https://www.chessprogramming.org/Bitboards)
    - [Zobrist hashing](https://en.wikipedia.org/wiki/Zobrist_hashing)
- Search
    - [Principal Variation Search](https://en.wikipedia.org/wiki/Principal_variation_search)
    - [Quiescence search](https://en.wikipedia.org/wiki/Quiescence_search)
    - [Iterative deepening](https://www.chessprogramming.org/Iterative_Deepening)
    - [Triangular PV-table](https://www.chessprogramming.org/Triangular_PV-Table)
    - Move ordering ([principal variation](https://www.chessprogramming.org/Principal_Variation), [MVV-LVA](https://www.chessprogramming.org/MVV-LVA))
    - [Check extensions](https://www.chessprogramming.org/Check_Extensions)
    - [Null move pruning](https://en.wikipedia.org/wiki/Null-move_heuristic)
- Evaluation
    - Opening/endgame [piece square tables](https://www.chessprogramming.org/Piece-Square_Tables)
    - Passed pawn bonus
    - [Mop-up evaluation](https://www.chessprogramming.org/Mop-up_Evaluation)
- Interface
    - [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) compatible
    - Bulk [perft](https://www.chessprogramming.org/Perft) testing

## :hibiscus: Build

```bash
git clone https://github.com/joey-bednar/amelia && \
cd amelia && \
make && \
./build/amelia
```
