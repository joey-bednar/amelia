# Changelog

## [v0.6](https://github.com/joey-bednar/annie/tree/v0.6) (6/30/24)

Engine first searches the principal variation determined by the previous
search. This is allows the search to prune more branches and reach higher
depths faster.

- Added PV-table to retrieve PV moves
- Updated move ordering to prioritize PV moves first
- Fixed threefold repetition detection
- Added perft to UCI input
- Adjust time management settings
- Fixed rare illegal PV move shown

**v0.6 vs v0.5:** 105 wins, 83 draws, 12 losses (0.733)

**Elo increase:** 175.0 +/- 37.5

## [v0.5](https://github.com/joey-bednar/annie/tree/v0.5) (6/27/24)

Engine evaluates all captures in the position at horizon nodes and uses
iterative deepening to display search info/manage time.

- Fixed illegal move promoting to knight
- Added search info showing depth, eval, nodes, time, nps, and pv
- Search captures first for all plys
- Added Quiescence search
- Added basic logic for time management
- Added ending search early if found mate

**v0.5 vs v0.4:** 78 wins, 21 draws, 1 losses (0.885)

**Elo increase:** 354.5 +/- 77.6

## [v0.4](https://github.com/joey-bednar/annie/tree/v0.4) (6/24/24)

Engine avoids threefold repetitions to push for wins when better and plays
significantly faster.

- Added Zobrist hashing and records positions to check for threefold repetition.
- Added fullmove/halfmove counter to check for fifty-move rule.
- Fixed illegal moves after loading 100+ move position in UCI
- Compiled with O3 optimization flag.
- Run search using alpha-beta at root node to prune more branches.
- Increased efficiency of make/unmake functions.

**v0.4 vs v0.3:** 42 wins, 35 draws, 23 losses (0.595)

**Elo increase:** 66.8 +/- 55.8

## [v0.3](https://github.com/joey-bednar/annie/tree/v0.3) (5/15/24)

Engine can search to greater depth using alpha-beta pruning and does not resign.

- Added alpha-beta pruning. Increase from depth 3 to depth 5 due to saved search time.
- Fixed illegal moves due to promotion/losing position. Does not play on after mate/stalemate.
- Added eval/mate UCI info outputs.

**v0.3 vs v0.2:** 57 wins, 43 draws, 0 losses (0.785)

**Elo increase:** 225.0 +/- 50.6

## [v0.2](https://github.com/joey-bednar/annie/tree/v0.2) (5/13/24)

Engine places pieces on more normal squares.

- Plays on depth 3.
- Added piece square tables.
- Makes some illegal moves when losing badly resulting in resignation.

**v0.2 vs v0.1:** 25 wins, 63 draws, 12 losses (0.565)

**Elo increase:** 45.4 +/- 41.3

## [v0.1](https://github.com/joey-bednar/annie/tree/v0.1) (5/13/24)

First working release on lichess. 

- Working UCI protocol with no outputs
- Understands material count and mate only.
- Opening/middlegame is random. Does not push for win in endgame.
- Missing many basic features (draw conditions,some illegal moves).

