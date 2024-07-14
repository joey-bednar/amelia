# Changelog

## [v0.9](https://github.com/joey-bednar/annie/tree/v0.9) (7/14/24)

Search efficiency was greatly improved using pruning/extension techniques.

- Added null move pruning
- Added check extensions
- Fix KNN vs K not forced draw by FIDE rules

**v0.9 vs v0.8:** 286 wins, 93 draws, 121 losses (0.612)

**Elo increase:** 119.1 +/- 28.7

## [v0.8](https://github.com/joey-bednar/annie/tree/v0.8) (7/13/24)

Improved move generation and time management.

- Added emergency stop to search after cutoff time
- Added increment to time management calculations
- Use precomputed attack rays to search for attacks/checks
- Refactor of move generation to compress move data
- Switch insufficient material rules from USCF to FIDE

**v0.8 vs v0.7:** 254 wins, 104 draws, 142 losses (0.612)

**Elo increase:** 79.2 +/- 27.6

## [v0.7](https://github.com/joey-bednar/annie/tree/v0.7) (7/2/24)

Engine can now deliver basic checkmates and use its king in endgames.

- Added Mop-up evaluation
- Added insufficient material draw condition to search
- Fixed perft function

**v0.7 vs v0.6:** 243 wins, 111 draws, 146 losses (0.597)

**Elo increase:** 68.3 +/- 27.3

## [v0.6](https://github.com/joey-bednar/annie/tree/v0.6) (6/30/24)

Engine first checks the principal variation determined by the previous
search. This allows for more branches to be pruned resulting in faster
search.

- Added PV-table to retrieve PV moves
- Updated move ordering to prioritize PV moves first
- Fixed threefold repetition detection
- Added perft to UCI input
- Adjust time management settings
- Fixed rare illegal PV move shown

**v0.6 vs v0.5:** 266 wins, 195 draws, 39 losses (0.727)

**Elo increase:** 170.1 +/- 24.4

## [v0.5](https://github.com/joey-bednar/annie/tree/v0.5) (6/27/24)

Engine evaluates all captures in the position at horizon nodes and uses
iterative deepening to display search info/manage time.

- Fixed illegal move promoting to knight
- Added search info showing depth, eval, nodes, time, nps, and pv
- Search captures first for all plys
- Added Quiescence search
- Added basic logic for time management
- Added ending search early if found mate

**v0.5 vs v0.4:** 395 wins, 98 draws, 7 losses (0.888)

**Elo increase:** 359.7 +/- 34.7

## [v0.4](https://github.com/joey-bednar/annie/tree/v0.4) (6/24/24)

Engine avoids threefold repetitions to push for wins when better and plays
significantly faster.

- Added Zobrist hashing and records positions to check for threefold repetition.
- Added fullmove/halfmove counter to check for fifty-move rule.
- Fixed illegal moves after loading 100+ move position in UCI
- Compiled with O3 optimization flag.
- Run search using alpha-beta at root node to prune more branches.
- Increased efficiency of make/unmake functions.

**v0.4 vs v0.3:** 216 wins, 178 draws, 106 losses (0.610)

**Elo increase:** 77.7 +/- 24.7

## [v0.3](https://github.com/joey-bednar/annie/tree/v0.3) (5/15/24)

Engine can search to greater depth using alpha-beta pruning and does not resign.

- Added alpha-beta pruning. Increase from depth 3 to depth 5 due to saved search time.
- Fixed illegal moves due to promotion/losing position. Does not play on after mate/stalemate.
- Added eval/mate UCI info outputs.

**v0.3 vs v0.2:** 307 wins, 189 draws, 4 losses (0.803)

**Elo increase:** 244.1 +/- 24.3

## [v0.2](https://github.com/joey-bednar/annie/tree/v0.2) (5/13/24)

Engine places pieces on more normal squares.

- Plays on depth 3.
- Added piece square tables.
- Makes some illegal moves when losing badly resulting in resignation.

**v0.2 vs v0.1:** 95 wins, 360 draws, 45 losses (0.550)

**Elo increase:** 34.9 +/- 16.0

## [v0.1](https://github.com/joey-bednar/annie/tree/v0.1) (5/13/24)

First working release on lichess. 

- Working UCI protocol with no outputs
- Understands material count and mate only.
- Opening/middlegame is random. Does not push for win in endgame.
- Missing many basic features (draw conditions,some illegal moves).

