# Changelog

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

