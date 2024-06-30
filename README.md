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
- [Piece square tables](https://www.chessprogramming.org/Piece-Square_Tables)
- [Negamax](https://en.wikipedia.org/wiki/Negamax) search
- [De Bruijn bitscan](https://en.wikipedia.org/wiki/De_Bruijn_sequence)
- Bulk [perft](https://www.chessprogramming.org/Perft) testing
- [UCI](https://en.wikipedia.org/wiki/Universal_Chess_Interface) compatible

## Build

```bash
git clone https://github.com/joey-bednar/annie && \
cd annie && \
make && \
./build/annie
```

## SCID vs PC

[SCID vs PC](https://scidvspc.sourceforge.net/) is a chess interface used to run chess engines, analyze lines, play against engines,
or host engine vs engine tournaments.

##### Installation (Ubuntu)

- Install dependencies: `sudo apt install tcl tk tcl-dev tk-dev gcc g++ `
- Go to the [SCID vs PC](https://scidvspc.sourceforge.net/), click `Downloads` and download the `Source` file.
- Build from source:

```bash
tar -xzf scid_vs_pc-4.25.tgz
cd scid_vs_pc-4
./configure
sudo make install
```

- Run using `/usr/local/bin/scid`
- Click `Tools`, `Analysis Engines`, `New`, `Browse`, and select the engine executable.
- Click `Configure`, `Save`, `OK`

##### Play Human vs Engine

- Click `Play`, `Computer - UCI Engine`, and choose your game settings.
- Click `Play` to start the game.

##### Play Engine vs Engine

- Click `Play`, `Computer Tournament`
- Set the `Number of Engines` to `2` and select the two engines you would like to play each other.
- Click `Ok`.

> Optional: Install [Stockfish](https://stockfishchess.org/download/) or other engines to play against.
