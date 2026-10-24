# Heirs Agent

A game-playing agent for **Heirs**, the 12×12 two-player strategy board game defined in
`csci561-sp26.pdf` (CSCI-561 Homework 2, Spring 2026). The agent reads the current board
state from `input.txt` and writes its chosen move to `output.txt`, per the I/O contract
specified in the assignment.

## Game summary

- 12×12 board, 24 pieces per side (`Baby`, `Prince`, `Princess`, `Pony`, `Guard`, `Tutor`,
  `Scout`, `Sibling`), White moves first.
- Win condition: capture the opponent's Prince. No check/checkmate, no promotions, no other
  special-case rules.
- Columns are labeled `a–n`, skipping `i` and `l` to avoid ambiguity.
- Each side has a total play-time budget (commonly 300s) that must cover every move for the
  entire game; running out of time or producing an illegal/missing move is an automatic loss.

Full piece movement rules are in `csci561-sp26.pdf`.

## Implementation (`homework2.cpp`)

- **Search**: minimax with alpha-beta pruning, driven by iterative deepening (depth 1, 2, 3, …)
  so the engine always returns the best move found at the deepest *fully completed* ply before
  its time budget runs out.
- **Time management**: wall-clock time is tracked with `std::chrono` from process start. Each
  move is allotted a fraction of the remaining budget (capped per move) with a safety reserve
  for I/O overhead, and a fast fallback path returns a move immediately when very little time
  remains, so the agent never risks being killed for exceeding its allotted time.
- **Move ordering**: captures are generated separately from quiet moves and sorted by captured-piece
  value (MVV) ahead of quiet moves; the previous iteration's best move is re-tried first at the
  next depth. Both improve alpha-beta cutoff rates.
- **Evaluation function**: material value per piece type, a board-centrality bonus for
  non-Prince pieces, and an advancement bonus for Babies. The Prince is weighted far above any
  other piece so that king safety dominates search decisions without needing a separate
  special-case rule.
- **Correctness fixes over earlier drafts**: Baby can now capture at its 2-square-forward
  distance (previously only the 1-square-forward capture was recognized), and forward direction
  for Baby/Scout is derived consistently from the board's row-index convention for both colors.

## Design goal

The agent is engineered to beat the two TA reference agents described in the assignment — a
random-legal-move agent and a simple minimax agent without alpha-beta pruning — by searching
materially deeper within the same time budget (alpha-beta + move ordering) and by playing a
better-informed positional game (centrality/mobility-aware evaluation) rather than material
counting alone. The same qualities carry over to being competitive against classmates' agents
in the class-wide competition.

## Results

- vs. random agent: **10/10**
- vs. simple minimax agent (no alpha-beta): **10/10**
- Class competition: **ranked top 10**

## Files

- `homework2.cpp` — the agent (C++17, reads `input.txt`, writes `output.txt`, no CLI args).
- `homework.py` — a Python draft of the same agent.
- `csci561-sp26.pdf` — the assignment specification.

## Build & run

```sh
g++ -O2 -std=c++17 -o homework2 homework2.cpp
# with input.txt present in the same directory:
./homework2
# reads input.txt, writes output.txt
```
