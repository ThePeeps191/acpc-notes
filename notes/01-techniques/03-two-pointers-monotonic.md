# Two Pointers, Monotonic Stack, Monotonic Deque

## Two pointers / sliding window

Valid when, for each left end `l`, the best/maximal right end `r(l)` is non-decreasing in `l` (window property is monotone: shrinking a valid window keeps it valid).

```cpp
// longest window with at most k distinct values
int longestKDistinct(const vector<int>& a, int k) {
    unordered_map<int, int> cnt;
    int best = 0;
    for (int l = 0, r = 0; r < sz(a); ++r) {
        cnt[a[r]]++;
        while (sz(cnt) > k) { if (--cnt[a[l]] == 0) cnt.erase(a[l]); ++l; }
        best = max(best, r - l + 1);
    }
    return best;
}
```

- Count windows satisfying a monotone property: for each r, all l' ≥ l(r) work → `ans += r - l + 1`.
- "Exactly k" = atMost(k) − atMost(k−1).
- Two sorted arrays: pairs with a_i + b_j ≤ X — one pointer forward on a, one backward on b.
- **Does not work with negative numbers for sums** (window sum isn't monotone) — use prefix sums + sorted structure instead.

## Monotonic stack: nearest greater / smaller

```cpp
// prv[i] = largest j < i with a[j] >= a[i] (or -1); nxt[i] = smallest j > i with a[j] > a[i] (or n)
// (>= on one side, > on the other: ties are counted exactly once in "subarrays where a[i] is max")
void nearestGreater(const vector<ll>& a, vector<int>& prv, vector<int>& nxt) {
    int n = sz(a); prv.assign(n, -1); nxt.assign(n, n);
    vector<int> st;
    rep(i, 0, n) {
        while (!st.empty() && a[st.back()] < a[i]) { nxt[st.back()] = i; st.pop_back(); }
        prv[i] = st.empty() ? -1 : st.back();
        st.push_back(i);
    }
}
```

Uses: largest rectangle in histogram (nearest smaller on both sides, area h_i·(R−L−1)); sum of subarray maxima/minima (contribution, §2.2); ACPC 2024 *Topographic Isolation* (for each peak, nearest index on the left and right with height ≥ peak: answer = min distance); visibility ("how many people can i see"); next greater element in a circular array (iterate 2n).

**Maximal all-ones rectangle in a 0/1 grid:** for each row keep `h[j]` = consecutive ones above; run histogram per row. O(nm).

## Monotonic deque: sliding window min/max

```cpp
// mins[i] = min(a[i-k+1..i]) for i >= k-1
vector<ll> slidingMin(const vector<ll>& a, int k) {
    deque<int> dq; vector<ll> res;
    rep(i, 0, sz(a)) {
        while (!dq.empty() && a[dq.back()] >= a[i]) dq.pop_back();
        dq.push_back(i);
        if (dq.front() <= i - k) dq.pop_front();
        if (i >= k - 1) res.push_back(a[dq.front()]);
    }
    return res;
}
```

- **DP optimisation:** `dp[i] = min_{i-k ≤ j < i} dp[j] + c_i` → keep deque of j by dp value. Also `dp[i] = max_{j} (dp[j] − f(j)) + g(i)` with a sliding range of j.
- 2D sliding window min: apply 1D on rows, then on columns of the result.
- Shortest subarray with sum ≥ K with negative numbers: deque over prefix sums (increasing), pop front while `p[i] − p[front] ≥ K`.

## Other amortised patterns

- **Stack simulation**: bracket matching, "remove adjacent equal pairs", evaluating expressions (shunting-yard).
- **Next pointer / "jump" with path compression**: "find next unvisited index ≥ i" — DSU where `nxt[i] = i+1` after visiting i; O(α) per query. Great for "paint ranges, each cell painted once" problems.
- **Small-to-large merging**: merge smaller set into larger → O(n log n) total insertions (§3.4).
