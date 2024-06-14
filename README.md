# Annie Chess Engine

## About

Annie is a [UCI](https://www.chessprogramming.org/UCI) chess engine written
from scratch in C. You can play Annie on [Lichess.org](https://lichess.org/@/AnnieEdison) or
play locally using a chess interface like [SCID vs PC](https://scidvspc.sourceforge.net/).

## Lichess

Challenge Annie on Lichess under the username [AnnieEdison](https://lichess.org/@/AnnieEdison).
You can also [spectate active games](https://lichess.org/@/AnnieEdison/tv).

## Technical Features

### Board Representation and Move Generation

- Bitboards
    - Precomputed King/Knight/Pawn attack maps
    - Attack maps using shifting bitboards
    - De Bruijn bitscan
- Bulk Perft testing

### Search

- Negamax

### Evaluation

- Bitscan material count
- Piece square tables

### Protocol

- Universal Chess Interface

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
