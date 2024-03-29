# Chess Engine

<!--
## Setup

### Engine

- Clone this repo.
- Run `make` to compile.
- Run `./build/main` to start the engine.

-->
### GUI

#### Installation

- Install dependencies: `sudo apt install tcl tk tcl-dev tk-dev gcc g++ `
- Go to the [SCID vs PC](https://scidvspc.sourceforge.net/), click `Downloads` and download the `Source` file.
- Build from source:
```
tar -xzf scid_vs_pc-4.24.tgz
cd scid_vs_pc-4
./configure
sudo make install
```
- Run using `/usr/local/bin/scid`
- Click `Tools`, `Analysis Engines`, `New`, `Browse`, and select the engine executable.
- Click `Configure`, `Save`, `OK`

#### Play Human vs Engine

- Click `Play`, `Computer - UCI Engine`, and choose your game settings.
- Click `Play` to start the game.

#### Play Engine vs Engine

- Click `Play`, `Computer Tournament`
- Set the `Number of Engines` to `2` and select the two engines you would like to play each other.
- Click `Ok`.

> Optional: Install [Stockfish](https://stockfishchess.org/download/) or other engines to play against.
