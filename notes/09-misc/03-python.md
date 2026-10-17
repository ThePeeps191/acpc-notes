# Python as a Backup Language

Use Python for: arbitrary-precision integers, exact fractions, quick string manipulation, one-off math. Avoid it for heavy loops (≥ 10^7 operations) unless PyPy is used.

```python
import sys, math
from fractions import Fraction
from decimal import Decimal, getcontext
from functools import lru_cache
from itertools import permutations, combinations, product, accumulate
from collections import deque, defaultdict, Counter
import heapq, bisect

input = sys.stdin.readline                      # fast line input
data = sys.stdin.buffer.read().split()          # fastest: all tokens at once (bytes)
sys.setrecursionlimit(1 << 20)                  # deep recursion (may also need threading stack size)

# exact big integers
x = 10**100 + 7; y = pow(3, 10**18, x)         # modular power
inv = pow(5, -1, 10**9 + 7)                     # modular inverse (Python 3.8+)
r = math.isqrt(10**30)                          # exact floor sqrt
c = math.comb(100, 50); p = math.perm(10, 3)    # binomial / permutations
g = math.gcd(12, 18); l = math.lcm(4, 6)        # lcm: Python 3.9+

# exact rationals / high precision decimals
f = Fraction(1, 3) + Fraction(1, 6)             # 1/2
getcontext().prec = 100; d = Decimal(2).sqrt()

# memoised recursion
@lru_cache(maxsize=None)
def ways(n):
    return 1 if n <= 1 else ways(n - 1) + ways(n - 2)

# output many lines quickly
out = []
for i in range(5): out.append(str(i))
sys.stdout.write("\n".join(out) + "\n")

# formatting
print(f"{3.14159265:.6f}")                      # fixed 6 decimals
print(int("ff", 16), bin(10)[2:], format(255, "x"))
```

- `heapq` is a min-heap; push `(-priority, item)` for a max-heap.
- `bisect.bisect_left(a, x)` = `lower_bound`; `bisect_right` = `upper_bound`.
- Deep recursion in CPython crashes even with a high limit; run the solver in a `threading.Thread` with `threading.stack_size(1 << 27)`, or rewrite iteratively.
- Strings are immutable: build with a list and `"".join(...)`.
- Integer division floors toward −∞ (`-7 // 2 == -4`), unlike C++ (truncates toward 0). `%` result has the sign of the divisor.
