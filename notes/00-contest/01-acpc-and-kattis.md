# ACPC, Kattis, and Team Strategy

**How to use these notes.** Stuck on a problem → go to *When You're Stuck* (§1.3): constraint → complexity table, then keyword → technique table, which points to the section with code. Every C++ block compiles as-is after the template (§1.2) with `#include <bits/stdc++.h>`, `using namespace std;`, `using ll = long long;`. Blocks marked **[stress-tested]** were checked against brute force; the rest compile and were hand-verified, so still test on samples.

## What ACPC looks like (from all sets 2016–2024 on Kattis)

- 8–15 problems per set. Kattis difficulty (1–10 scale) spread is very consistent:
  - **~35% easy (1.2–2.9):** implementation, careful reading, simulation. Solve these *fast and without WA* — penalty time decides ranks among teams with equal solves.
  - **~35% medium (3.0–5.0):** one standard idea: binary search on answer + greedy, monotonic stack, Dijkstra/BFS with a twist, tree DP, counting mod p, interval DP, grid flood fill, offline DSU.
  - **~30% hard (5.1–8.3):** probability/expected-value DP, flows with modelling (min flow with lower bounds, assignment), FFT, number theory (factoring, gcd tricks), interactive problems with tiny query budgets, geometry + search, string canonical forms, nasty interval DP.
- Expert/CM level: easy+medium ≈ CF Div2 A–C; hard ≈ Div2 D–F / Div1 C–D. Hard ACPC problems are usually *one known technique plus modelling* rather than research-level tricks — recognizing the model is the whole game. See §1.4 for every past problem mapped to its technique.
- Themes recur: Kattis "Medium/Hard" problems from ACPC very often involve **probability**, **flows/matching**, **Dijkstra on augmented state**, **binary search on the answer**, and **number theory**.

## Kattis specifics

- Language: C++ compiled with GCC, `-O2`, gnu++17 or newer, 64-bit. `__int128`, `__builtin_*`, `<bits/stdc++.h>`, `__gnu_pbds` all work.
- Time limit is **per test file**; there may be dozens of files. Memory is usually 1024 MB (check the problem page).
- Verdicts stop at the first failing test file — you don't learn which case failed. A WA on test 1 means the sample itself fails (or format is wrong).
- Output: trailing whitespace/newline differences are ignored. Floating answers are accepted within the stated absolute/relative error; print with `fixed << setprecision(10)`.
- **Interactive problems** (ACPC 2024 had two): flush after every query (`cout << ... << endl` or `cout.flush()`), never read past what the judge sends, terminate immediately after the final answer. See §10.1.
- Multiple test cases inside one file (`T` given): **reset all global state**.
- Python: big integers are trivial in Python; consider it for one-off big-number problems (e.g. ACPC 2017 *Divide by 100* has numbers with 10^6 digits — just string manipulation though).
- Recursion: Kattis has a generous stack, but DFS depth 10^6 in C++ is still risky; prefer iterative for path graphs with n ≥ 10^6.

## Team strategy (2–3 people, one machine)

1. **First 15 min:** split the problem set; everyone reads, labels each problem `easy / medium / hard / unknown` and writes one-line summary + constraints on paper. Read *all* problems — ACPC difficulty is not ordered by letter.
2. **Keyboard discipline:** only one person types; others think on paper. Never debug at the keyboard while someone with a ready solution waits — print/write the code and debug on paper.
3. Watch the scoreboard: problems solved by many teams are easier than they look. If 10 teams solved X and you think it's hard, you're missing something simple.
4. **Before submitting:** run all samples, a tiny edge case (n = 1, all equal, max values), and check output format. One WA costs 20 minutes of penalty.
5. **After WA:** reread the statement *first* (most WAs are misreads), then check §1.3 bug list, then write a brute force and compare on random small inputs.
6. **Last hour:** stop starting new hard problems unless the idea is complete; finish/debug what's closest. Freeze-time submissions still count.
7. Two people can pair on the hardest problem: one explains the problem from scratch to the other — this exposes wrong assumptions.

## Precision and number sizes (memorize)

| type | max | notes |
|---|---|---|
| `int` | 2.1·10^9 | `a*b` overflows when both ~ 5·10^4 |
| `long long` | 9.2·10^18 | product of two 10^9 values fits; two 10^10 doesn't |
| `__int128` | 1.7·10^38 | multiply two `ll` safely; no I/O, write a printer |
| `double` | 15–16 digits | integers exact up to 2^53 ≈ 9·10^15 |
| `long double` | 18–19 digits | 80-bit on x86 GCC; slower but helps precision |

- Operations/second: ~ 3·10^8 simple ops in 1 s with `-O2`; `set/map` ~ 10^7 ops/s; `unordered_map` ~ 3·10^7 with reserve; `cin` with sync off ~ 10^7 ints/s.
- Memory: `int` array of 10^8 = 400 MB. `vector<vector<int>>` with 10^6 small vectors has big overhead — use CSR / flat arrays.
