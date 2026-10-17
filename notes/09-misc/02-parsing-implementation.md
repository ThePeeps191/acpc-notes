# Parsing, Simulation, Dates, Big Numbers

## Reading input safely

```cpp
// frag
string line;
getline(cin, line);                       // whole line (after cin >> x, first call cin.ignore())
if (!line.empty() && line.back() == '\r') line.pop_back();   // Windows line endings
stringstream ss(line); vector<string> tok; string w;
while (ss >> w) tok.push_back(w);         // split on whitespace
// split on a custom delimiter
stringstream s2(line); string part;
while (getline(s2, part, ',')) { /* use part */ }
```

- Unknown number of test cases: `while (cin >> n)`. Lines until a sentinel: check before processing.
- Numbers with decimals that must be exact (money, "two decimal places"): read as string, convert to integer cents.
- Characters of a grid: read rows as strings; `vector<string> g(R)`; pad with a border of walls to avoid bounds checks.

## Expression parsing (recursive descent)

Grammar with precedence: `expr = term (('+'|'-') term)*`, `term = factor (('*'|'/') factor)*`, `factor = number | '(' expr ')' | '-' factor`. Add a level per precedence class; right-associative operators (`^`) recurse on the right.

```cpp
// [stress-tested] integers, + - * / ^ (right assoc), unary minus, parentheses, spaces allowed
struct Parser {
    string s; size_t i = 0;
    Parser(string str) { for (char c : str) if (c != ' ') s += c; }
    char peek() { return i < s.size() ? s[i] : 0; }
    ll expr() {
        ll v = term();
        while (peek() == '+' || peek() == '-') { char op = s[i++]; ll r = term(); v = op == '+' ? v + r : v - r; }
        return v;
    }
    ll term() {
        ll v = unary();
        while (peek() == '*' || peek() == '/') { char op = s[i++]; ll r = unary(); v = op == '*' ? v * r : v / r; }
        return v;
    }
    ll unary() { if (peek() == '-') { ++i; return -unary(); } if (peek() == '+') { ++i; return unary(); } return power(); }
    ll power() {
        ll b = atom();
        if (peek() == '^') { ++i; ll e = unary(); ll r = 1; while (e-- > 0) r *= b; return r; }
        return b;
    }
    ll atom() {
        if (peek() == '(') { ++i; ll v = expr(); ++i; return v; }   // skip ')'
        ll v = 0; while (isdigit(peek())) v = v * 10 + (s[i++] - '0');
        return v;
    }
};
// usage: Parser p("2*(3+4)-5"); ll v = p.expr();
```

- **Mini-languages** (ACPC 2016 *Modelling Problems*: assignments, conditionals, assertions with nested blocks): same idea — one function per syntactic construct, each consumes its characters and returns a node/value. Build an AST if the program must be run many times (e.g. once per assignment of unknown bits: brute force 2^k executions).
- Evaluate the parse for *all* parenthesisations → that's interval DP (§4.1), not a parser.
- Shunting-yard (two stacks: values, operators) is an alternative when the grammar is flat.

## Simulation tips

- Write the simulation exactly as the statement describes first (it's the brute force), then optimise only the hot part.
- **Event simulation:** priority queue of (time, event); process in time order; ties broken exactly as the statement says.
- **Skip ahead:** if the state repeats (cycle detection, §5.2) or many steps do nothing (jump to the next interesting time), skip them in O(1).
- Grids: `dr/dc` arrays; direction turning `d = (d + 1) % 4` (clockwise if order is N, E, S, W).
- Circular lists with removals (players leaving a game, Josephus): `std::list` or arrays `nxt/prv`; or a BIT/order-statistics structure to find the k-th alive player in O(log n).
- Josephus (every k-th removed, 0-indexed survivor): `J(1) = 0; J(n) = (J(n−1) + k) % n`.

## Dates and times

```cpp
bool leap(int y) { return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0; }
int daysInMonth(int y, int m) { static const int d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; return d[m - 1] + (m == 2 && leap(y)); }
// days since a fixed epoch (proleptic Gregorian); difference of two dates = difference of these
ll dayNumber(ll y, ll m, ll d) {
    if (m < 3) { y--; m += 12; }
    return 365 * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + d - 1;
}
// day of week: (dayNumber(y, m, d) + 2) % 7 gives 0 = Monday ... 6 = Sunday
```

- Clock problems: work in seconds (or the LCM of the hand periods) to keep integers; hand angles: hour 0.5°/min, minute 6°/min, second 6°/s. Hands align at multiples of 12/11 hours (hour–minute) and 60/59 minutes (minute–second) — use exact fractions.
- Time zones / 24h wraparound: normalise with `((t % M) + M) % M`.

## Big numbers without Python

- Use strings of digits; add/subtract/multiply-by-small/divide-by-small in O(n). Multiplying two big numbers: O(n·m) schoolbook or FFT (§7.5).
- Division by 10^k is just moving the decimal point (ACPC 2017 *Divide by 100*); strip trailing zeros and a trailing '.'.
- Comparing huge numbers: compare lengths first, then lexicographically.
- `__int128` covers up to ~1.7·10^38.

## Base conversion and misc

- To base b: repeated `% b`, `/ b`, reverse. Negative base −2: same loop but fix negative remainders (`r += 2; n += 1`).
- Roman numerals: greedy with the table {1000 M, 900 CM, 500 D, 400 CD, 100 C, 90 XC, 50 L, 40 XL, 10 X, 9 IX, 5 V, 4 IV, 1 I}.
- Rotate grid 90° clockwise: `res[j][R − 1 − i] = g[i][j]`. Transpose: `res[j][i] = g[i][j]`.
- Rounding to nearest integer: `llround(x)`; to k decimals: print with `setprecision(k)` (banker's rounding is not used by printf; halfway cases depend on binary representation — add 1e-9 if the statement demands "round half up").
