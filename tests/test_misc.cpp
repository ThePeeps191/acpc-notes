// uses: 09-misc/02-parsing-implementation.md Parser leap daysInMonth dayNumber
mt19937 rg(5);
int R(int a, int b) { return uniform_int_distribution<int>(a, b)(rg); }
#define CHECK(c) if (!(c)) { printf("fail line %d iter %d\n", __LINE__, it); return 1; }
// random expression tree printed with minimal-but-correct parentheses; value computed directly
struct Node { char op; ll v; int l, r; };
vector<Node> T;
int gen(int d) {
    if (d == 0 || R(0, 3) == 0) { T.push_back({'n', R(0, 9), -1, -1}); return sz(T) - 1; }
    if (R(0, 5) == 0) { int c = gen(d - 1); T.push_back({'u', 0, c, -1}); return sz(T) - 1; }
    char ops[] = {'+', '-', '*'}; char op = ops[R(0, 2)];
    int a = gen(d - 1), b = gen(d - 1); T.push_back({op, 0, a, b}); return sz(T) - 1;
}
ll val(int x) { auto& n = T[x]; if (n.op == 'n') return n.v; if (n.op == 'u') return -val(n.l);
    ll a = val(n.l), b = val(n.r); return n.op == '+' ? a + b : n.op == '-' ? a - b : a * b; }
int prec(char op) { return op == 'n' ? 9 : op == 'u' ? 3 : op == '*' ? 2 : 1; }
string str(int x) {
    auto& n = T[x];
    if (n.op == 'n') return to_string(n.v);
    if (n.op == 'u') { string c = str(n.l); if (prec(T[n.l].op) < 3) c = "(" + c + ")"; return "-" + c; }
    string a = str(n.l), b = str(n.r);
    if (prec(T[n.l].op) < prec(n.op)) a = "(" + a + ")";
    if (prec(T[n.r].op) < prec(n.op) || (prec(T[n.r].op) == prec(n.op) && n.op != '*' && n.op != '+')) b = "(" + b + ")";
    if (prec(T[n.r].op) == prec(n.op) && n.op == '+' && T[n.r].op != '+') {}
    return a + string(" ") + n.op + " " + b;
}
int main() {
    for (int it = 0; it < 20000; ++it) {
        T.clear(); int root = gen(R(0, 6)); string s = str(root);
        Parser p(s); CHECK(p.expr() == val(root));
    }
    int it = 0;
    { Parser p("2^3^2"); CHECK(p.expr() == 512); Parser q("-2^2"); CHECK(q.expr() == -4); Parser r("7/2*2"); CHECK(r.expr() == 6); }
    // dates: consecutive days differ by 1; 2000-01-01 is Saturday (5 with Monday = 0)
    ll prev = dayNumber(1899, 12, 31);
    for (int y = 1900; y <= 2100; ++y) rep(m, 1, 13) rep(d, 1, daysInMonth(y, m) + 1) { ll cur = dayNumber(y, m, d); CHECK(cur == prev + 1); prev = cur; }
    CHECK((dayNumber(2000, 1, 1) + 2) % 7 == 5);
    CHECK((dayNumber(2026, 10, 16) + 2) % 7 == 4);   // Friday
    puts("misc ok");
}
