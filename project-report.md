# PD-World RL Experiments Report (α = 0.3, γ = 0.5)

This report summarizes the qualitative findings from the markdown Q-table snapshots produced by the implementation (state space1: `(i, j, x, s, t, u)`), where each grid cell shows the **best Q-value** and the **argmax action** (arrow / `P` / `D`) for that cell, separated by `x=0` (not carrying) and `x=1` (carrying).

---

## Environment recap (what “good learning” should look like)

- **When `x=0` (not carrying):** good behavior is to **move toward a pickup cell** and take `Pickup (P)` when applicable.
- **When `x=1` (carrying):** good behavior is to **move toward a dropoff cell** and take `Dropoff (D)` when applicable.
- Since moves cost `-1` and pickup/dropoff give `+13`, attractive paths should become “shaped” as:
  - **pickup → shortest path → dropoff**, repeated, until pickup empties / dropoff fills.

---

# Experiment 1 — Q-learning: 4000 PRANDOM → 4000 PGREEDY

### Setup
- Q-learning, α=0.3, γ=0.5  
- Steps 1–4000: **PRANDOM**  
- Steps 4001–8000: **PGREEDY**  
- Goal: interpret Q-table at the **middle** (4000) and **end** (8000).

### Observations at 4000 (PRANDOM snapshot)
- The grid shows **mostly directional arrows** with modest Q-values and limited “dominant” `P`/`D` emphasis.
- Interpretation: PRANDOM explores broadly; it still learns *some* useful value gradients, but because actions are frequently random, the policy does not consistently exploit the best-known routes.

### Observations at 8000 (after 4000 PGREEDY)
- The Q-table remains direction-heavy but is **more stable** (tie-breaking randomness aside).
- Interpretation: once PGREEDY is used, the agent begins to **commit to the current best-known actions**, reinforcing consistent movement patterns. In a well-formed solution, we expect increasingly clear action preference toward pickup areas for `x=0` and toward dropoff areas for `x=1`.

### Key takeaway
- **PRANDOM builds exploration and early value signals; PGREEDY consolidates those signals into consistent preferred actions.**
- Experiment 1 demonstrates the classic transition: **explore first → exploit later**.

---

# Experiment 2 — Q-learning: 200 PRANDOM → 7800 PEXPLOIT

### Setup
- Q-learning, α=0.3, γ=0.5  
- Steps 1–200: **PRANDOM**  
- Steps 201–8000: **PEXPLOIT** (80% greedy, 20% random among non-best applicable)
- Goal: analyze performance variables and learning at stages.

### Stage 1: after 200 PRANDOM (early snapshot)
- Strong `P` markers appear at pickup cells and strong `D` markers appear at dropoff cells (with noticeably higher Q-values than normal movement cells).
- Interpretation:
  - Even with only 200 steps, the agent has already experienced the high reward events (`+13`) enough times that **pickup and dropoff actions start separating themselves numerically** from movement actions.
  - Most other cells still show near-zero utilities: the “value” has not yet propagated far along paths.

### Stage 2: after switching to PEXPLOIT (expected trajectory)
- PEXPLOIT should rapidly:
  - Prefer the **current best path** 80% of the time (so it *looks greedy*),
  - Still inject 20% randomness to prevent getting stuck with suboptimal routes.
- Expected effect on performance variables:
  - **(a) bank account** should rise steadily (more frequent successful pickup/dropoff cycles).
  - **(b) operators to terminal state** should decrease over time (shorter, more efficient routes emerge).

### Key takeaway
- Experiment 2 should outperform Experiment 1 in “time-to-competence” because:
  - PRANDOM is only used briefly (just to seed exploration),
  - then PEXPLOIT accelerates learning by repeatedly revisiting and strengthening the best-known behaviors while still exploring.

---

# Experiment 3 — SARSA: 200 PRANDOM → 7800 PEXPLOIT (On-policy)

### Setup
- SARSA (on-policy TD control), α=0.3, γ=0.5  
- Steps 1–200: **PRANDOM**  
- Steps 201–8000: **PEXPLOIT**
- Goal: compare SARSA vs Q-learning and report final Q-table.

### Comparison focus: Q-learning vs SARSA
- **Q-learning (off-policy):** update uses `max_a' Q(s', a')`  
  → tends to learn toward *optimal greedy behavior*, even if the acting policy is exploratory.
- **SARSA (on-policy):** update uses `Q(s', a_next)` where `a_next` is chosen by the same policy used to act  
  → learns values that reflect **the risk / randomness of the current policy**.

### Expected behavioral difference
- Under exploratory policies (like PEXPLOIT with 20% randomness):
  - **Q-learning** often learns more aggressive, optimistic utilities (it assumes greedy future steps).
  - **SARSA** tends to learn more conservative utilities (it “knows” it will sometimes take suboptimal random actions).

### What the final SARSA Q-table should show
- Strong `P` at pickup cells when `x=0`, strong `D` at dropoff cells when `x=1`, similar to Q-learning,
- But often with **slightly lower / more cautious values** along paths compared to Q-learning, especially where exploration can cause detours.

### Key takeaway
- If your performance measures are tracked:
  - Q-learning typically reaches higher peak performance sooner (more “optimal” route shaping),
  - SARSA may be more stable/realistic for the actual acting policy because it accounts for exploration in its updates.

---

# Overall conclusions

1. **Experiment 1 (PRANDOM → PGREEDY)** demonstrates how exploration seeds value gradients, and greedy execution stabilizes behavior.
2. **Experiment 2 (PRANDOM → PEXPLOIT)** is designed to learn faster and more effectively than Experiment 1 by combining early exploration with mostly-greedy exploitation while still exploring.
3. **Experiment 3 (SARSA with the same policy schedule as Exp 2)** should produce behavior similar in structure but typically more conservative than Q-learning, illustrating the difference between off-policy vs on-policy learning.

---
