#!/usr/bin/env python3
"""Stress tests for the code in notes/. Each tests/test_*.cpp starts with lines like
    // uses: 02-data-structures/03-segment-tree.md LazySeg
which pull the ```cpp block containing `struct LazySeg` / a function named LazySeg out of
the Markdown, so the tested code is exactly the printed code. Run: python tests/run_tests.py
"""
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))
from build import CPP_BLOCK, PRELUDE, find_gxx  # noqa: E402

BIN = ROOT / "tests" / "bin"


def block_for(md: str, name: str) -> str:
    text = (ROOT / "notes" / md).read_text()
    pat = re.compile(rf"(struct|class)\s+{re.escape(name)}\b|\b{re.escape(name)}\s*\(")
    for m in CPP_BLOCK.finditer(text):
        if pat.search(m.group(1)):
            return m.group(1)
    raise SystemExit(f"block '{name}' not found in {md}")


def main():
    gxx = find_gxx()
    BIN.mkdir(exist_ok=True)
    only = sys.argv[1:]
    fails = 0
    for t in sorted((ROOT / "tests").glob("test_*.cpp")):
        if only and not any(o in t.name for o in only):
            continue
        src = t.read_text()
        code, seen = [], set()
        for md, names in re.findall(r"^// uses: (\S+) (.+)$", src, re.M):
            for n in names.split():
                b = block_for(md, n)
                if b not in seen:
                    seen.add(b)
                    code.append(b)
        full = PRELUDE + "\n".join(code) + "\n" + src
        cpp = BIN / (t.stem + ".cpp")
        exe = BIN / t.stem
        cpp.write_text(full)
        r = subprocess.run([gxx, "-std=gnu++17", "-O2", "-fsanitize=address,undefined",
                            "-o", str(exe), str(cpp)], capture_output=True, text=True)
        if r.returncode:
            print(f"COMPILE FAIL {t.name}\n{r.stderr[:3000]}")
            fails += 1
            continue
        r = subprocess.run([str(exe)], capture_output=True, text=True, timeout=600)
        ok = r.returncode == 0
        fails += not ok
        print(f"{'ok  ' if ok else 'FAIL'} {t.name} {r.stdout.strip()[-300:]} {r.stderr.strip()[-1500:]}")
    sys.exit(1 if fails else 0)


if __name__ == "__main__":
    main()
