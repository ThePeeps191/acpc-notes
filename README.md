# ACPC Notes

Printable competitive-programming notes for the Alberta Collegiate Programming Contest (Kattis),
aimed at Codeforces Div2 B – Div1 D difficulty. Markdown sources in `notes/`, compiled into a single
PDF (`acpc-notes.pdf`) with syntax-highlighted C++, a contents page with page numbers, and PDF bookmarks.

Designed to complement the CSES book (*Competitive Programmer's Handbook*): less prose, more
ready-to-type code, modelling recipes, and "when stuck" lookup tables.

## Layout

```
notes/
  00-contest/           playbook: ACPC/Kattis facts, template, stuck guide,
                        every ACPC 2016-2024 problem -> technique, worked patterns
  01-techniques/        binary/ternary search, prefix sums & sweep, two pointers & monotonic
                        stack/deque, greedy, MITM & sqrt, bits, constructive/ad hoc
  02-data-structures/   DSU (+parity, rollback), Fenwick, segment trees (lazy), sparse table,
                        PBDS, treap, Mo, small-to-large
  03-dp/                patterns, bitmask/digit/SOS/profile, tree DP & rerooting,
                        optimisations (CHT, Li Chao, D&C, Knuth, aliens, slope), probability & games
  04-graphs/            shortest paths, MST/DAG/functional graphs, SCC/2-SAT/bridges/Euler,
                        LCA/HLD/centroid
  05-flows-matching/    Dinic + modelling (closure, lower bounds, min flow), Hopcroft-Karp/Konig,
                        Hungarian, min-cost flow
  06-math/              modular/CRT/binomials, primes & factorisation, counting, linear algebra,
                        FFT/NTT
  07-strings/           hashing, KMP, Z, Manacher, min rotation, trie, Aho-Corasick, SA, SAM
  08-geometry/          primitives, polygons, hull, calipers, closest pair, circles, integration
  09-misc/              interactive problems, parsing/simulation/dates, Python fallback
  <folder>/_part.txt    part title
tests/                  stress tests: code blocks are extracted from the Markdown and compared
                        against brute force (so the printed code is the tested code)
style/print.css         print stylesheet
build.py                Markdown -> HTML -> PDF (Playwright + Chrome), C++ block compile check
```

## Build

```bash
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
# uses the installed Google Chrome; without it run: .venv/bin/playwright install chromium
.venv/bin/python build.py            # -> acpc-notes.pdf
.venv/bin/python build.py --check    # compile every C++ block (g++ with bits/stdc++.h)
.venv/bin/python tests/run_tests.py  # brute-force stress tests of the templates
```

Code blocks assume the template prelude (`#include <bits/stdc++.h>`, `using namespace std;`,
`ll`, `ull`, `ld`, `pii`, `pll`, `all`, `sz`, `rep`, `INF`, `MOD`). First-line markers in a block:
`// frag` (statement fragment, wrapped in a function for checking), `// sketch` (not compiled).
