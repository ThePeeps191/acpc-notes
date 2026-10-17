#!/usr/bin/env python3
"""Compile notes/**/*.md into a single printable PDF (acpc-notes.pdf).

Usage:
    python build.py            # build acpc-notes.pdf
    python build.py --html     # also keep build/notes.html for debugging
    python build.py --check    # compile-check every ```cpp block with g++ (no PDF)

Pipeline: Markdown -> HTML (Pygments-coloured code) -> PDF via Playwright/Chrome.
The table of contents is rendered separately after the body so it can show real
page numbers (found by searching the body PDF's text), then prepended.
"""
import argparse
import html
import os
import re
import shutil
import subprocess
import sys
import tempfile
import unicodedata
from pathlib import Path

import markdown
from pygments.formatters import HtmlFormatter
from pypdf import PdfReader, PdfWriter

ROOT = Path(__file__).resolve().parent
NOTES = ROOT / "notes"
BUILD = ROOT / "build"
CSS = ROOT / "style" / "print.css"
OUT = ROOT / "acpc-notes.pdf"
TITLE = "ACPC Notes"

PYGMENTS_STYLE = "friendly"


# ---------------------------------------------------------------- collecting

def part_title(folder: Path) -> str:
    """Part title = first line of notes/NN-x/_part.txt, else derived from name."""
    meta = folder / "_part.txt"
    if meta.exists():
        return meta.read_text().strip().splitlines()[0]
    name = re.sub(r"^\d+-", "", folder.name)
    return name.replace("-", " ").title()


def collect():
    parts = []
    for folder in sorted(p for p in NOTES.iterdir() if p.is_dir()):
        files = sorted(folder.glob("*.md"))
        if files:
            parts.append((part_title(folder), files))
    return parts


# ---------------------------------------------------------------- markdown

def md_to_html(text: str) -> str:
    md = markdown.Markdown(
        extensions=["fenced_code", "codehilite", "tables", "attr_list", "sane_lists",
                    "md_in_html", "def_list"],
        extension_configs={"codehilite": {"css_class": "hl", "guess_lang": False,
                                          "pygments_style": PYGMENTS_STYLE}},
    )
    return md.convert(text)


HEAD_RE = re.compile(r"<h([1-5])([^>]*)>(.*?)</h\1>", re.S)
CODE_SPLIT = re.compile(r"(<pre.*?</pre>|<code.*?</code>)", re.S)
SUP_RE = re.compile(r"(?<=[A-Za-z0-9)\]])\^(\([^()]{1,20}\)|-?[A-Za-z0-9.]{1,8})")


def superscripts(body: str) -> str:
    """Prose `10^9`, `n^2`, `2^(n/2)` -> <sup>; code is left untouched."""
    parts = CODE_SPLIT.split(body)
    for i in range(0, len(parts), 2):
        parts[i] = SUP_RE.sub(lambda m: f"<sup>{m.group(1).strip('()') if m.group(1).startswith('(') else m.group(1)}</sup>", parts[i])
    return "".join(parts)


def strip_tags(s: str) -> str:
    return html.unescape(re.sub(r"<[^>]+>", "", s)).strip()


def build_body(parts):
    """Return (body_html, toc_entries). Headings are shifted down one level:
    folder = part (h1), file '#' = section (h2), '##' = subsection (h3)."""
    out = []
    toc = []  # (level, number, title, anchor)
    for pi, (ptitle, files) in enumerate(parts, 1):
        anchor = f"p{pi}"
        out.append(f'<h1 class="part" id="{anchor}">{pi}. {html.escape(ptitle)}</h1>')
        toc.append((1, f"{pi}", ptitle, anchor))
        for si, f in enumerate(files, 1):
            raw = f.read_text()
            body = superscripts(md_to_html(raw))
            sub = [0]

            def repl(m):
                lvl = int(m.group(1)) + 1
                inner = m.group(3)
                if lvl == 2:
                    num = f"{pi}.{si}"
                    a = f"s{pi}_{si}"
                    toc.append((2, num, strip_tags(inner), a))
                    return f'<h2 id="{a}"><span class="num">{num}</span> {inner}</h2>'
                if lvl == 3:
                    sub[0] += 1
                    num = f"{pi}.{si}.{sub[0]}"
                    a = f"s{pi}_{si}_{sub[0]}"
                    toc.append((3, num, strip_tags(inner), a))
                    return f'<h3 id="{a}"><span class="num">{num}</span> {inner}</h3>'
                return f"<h{lvl}{m.group(2)}>{inner}</h{lvl}>"

            body = HEAD_RE.sub(repl, body)
            out.append(f'<section class="file" data-src="{f.relative_to(ROOT)}">{body}</section>')
    return "\n".join(out), toc


def page_html(body: str, extra_class: str = "") -> str:
    css = CSS.read_text()
    pyg = HtmlFormatter(style=PYGMENTS_STYLE).get_style_defs(".hl")
    return (f"<!doctype html><html><head><meta charset='utf-8'><title>{TITLE}</title>"
            f"<style>{pyg}\n{css}</style></head>"
            f"<body class='{extra_class}'>{body}</body></html>")


# ---------------------------------------------------------------- pdf

FOOTER = ("<div style='font-size:7pt;width:100%;padding:0 0.45in;color:#666;"
          "font-family:Helvetica,Arial,sans-serif;display:flex;justify-content:space-between'>"
          f"<span>{TITLE}</span><span class='pageNumber'></span></div>")


def render_pdf(html_text: str, out: Path, numbered: bool):
    from playwright.sync_api import sync_playwright
    with tempfile.NamedTemporaryFile("w", suffix=".html", delete=False, dir=BUILD) as tf:
        tf.write(html_text)
        tmp = tf.name
    try:
        with sync_playwright() as p:
            try:
                browser = p.chromium.launch(channel="chrome")
            except Exception:
                browser = p.chromium.launch()  # needs `playwright install chromium`
            page = browser.new_page()
            page.goto(Path(tmp).as_uri(), wait_until="load")
            page.pdf(path=str(out), format="Letter", print_background=True,
                     display_header_footer=numbered,
                     header_template="<span></span>",
                     footer_template=FOOTER if numbered else "<span></span>",
                     margin={"top": "0.45in", "bottom": "0.5in" if numbered else "0.45in",
                             "left": "0.45in", "right": "0.45in"})
            browser.close()
    finally:
        os.unlink(tmp)


def norm(s: str) -> str:
    s = unicodedata.normalize("NFKC", s)  # undo ligatures (fi, ffl) in extracted text
    return re.sub(r"[^0-9a-z]", "", s.lower())


def find_pages(pdf: Path, toc):
    """Map each toc entry to the 1-based body page whose text contains it."""
    pages = [norm(pg.extract_text() or "") for pg in PdfReader(str(pdf)).pages]
    res, cur = [], 0
    for lvl, num, title, a in toc:
        key = norm(f"{num} {title}")[:40]
        found = None
        for i in range(cur, len(pages)):
            if key in pages[i]:
                found = i
                break
        if found is None:  # fall back: keep previous page, don't advance
            res.append(cur + 1)
            print(f"  warning: page not found for '{num} {title}'", file=sys.stderr)
        else:
            cur = found
            res.append(found + 1)
    return res


def build_toc_html(toc, pages):
    rows = []
    for (lvl, num, title, a), pg in zip(toc, pages):
        if lvl > 2:
            continue
        rows.append(f"<div class='toc{lvl}'><span class='tn'>{num}</span>"
                    f"<span class='tt'>{html.escape(title)}</span>"
                    f"<span class='tp'>{pg}</span></div>")
    return (f"<div class='toc'><div class='toc-title'>{TITLE} &mdash; Contents</div>"
            + "\n".join(rows) + "</div>")


def build_pdf(keep_html: bool):
    BUILD.mkdir(exist_ok=True)
    parts = collect()
    body, toc = build_body(parts)
    body_html = page_html(body)
    if keep_html:
        (BUILD / "notes.html").write_text(body_html)
    body_pdf = BUILD / "body.pdf"
    print("rendering body ...")
    render_pdf(body_html, body_pdf, numbered=True)
    pages = find_pages(body_pdf, toc)
    print("rendering contents ...")
    toc_pdf = BUILD / "toc.pdf"
    render_pdf(page_html(build_toc_html(toc, pages), "tocpage"), toc_pdf, numbered=False)

    w = PdfWriter()
    for pg in PdfReader(str(toc_pdf)).pages:
        w.add_page(pg)
    off = len(w.pages)
    for pg in PdfReader(str(body_pdf)).pages:
        w.add_page(pg)
    stack = {}
    for (lvl, num, title, a), pg in zip(toc, pages):
        parent = stack.get(lvl - 1) if lvl > 1 else None
        item = w.add_outline_item(f"{num} {title}", off + pg - 1, parent=parent)
        stack[lvl] = item
    w.write(str(OUT))
    print(f"wrote {OUT.name}: {off} contents pages + {len(w.pages) - off} body pages")


# ---------------------------------------------------------------- check

CPP_BLOCK = re.compile(r"^```(?:cpp|c\+\+)[^\n]*\n(.*?)^```", re.S | re.M)
PRELUDE = """#include <bits/stdc++.h>
using namespace std;
using ll = long long; using ull = unsigned long long; using ld = long double;
using pii = pair<int, int>; using pll = pair<ll, ll>;
#define all(x) begin(x), end(x)
#define sz(x) (int)(x).size()
#define rep(i, a, b) for (int i = (a); i < (b); ++i)
const ll INF = 4e18; const int MOD = 1e9 + 7;
"""


def find_gxx():
    for c in [os.environ.get("CXX"), "g++-16", "g++-15", "g++-14", "g++-13", "g++"]:
        if c and shutil.which(c):
            return c
    sys.exit("no g++ found (set CXX)")


def compile_ok(gxx, src, tmpdir):
    p = tmpdir / "a.cpp"
    p.write_text(src)
    r = subprocess.run([gxx, "-std=gnu++17", "-fsyntax-only", "-Wall", "-Wextra",
                        "-Wno-unused", "-Wno-sign-compare", "-Wno-unused-parameter", "-Wno-misleading-indentation",
                        str(p)], capture_output=True, text=True)
    return r.returncode == 0, r.stderr


def check():
    """Compile every ```cpp block (with the template prelude). First-line markers:
    `// frag` = statement fragment, wrapped in a function; `// sketch` / `// nocheck` =
    skipped. A block may use declarations from earlier blocks of the same file."""
    gxx = find_gxx()
    tmpdir = Path(tempfile.mkdtemp())
    total = bad = 0
    for f in sorted(NOTES.rglob("*.md")):
        text = f.read_text()
        earlier = []  # declaration blocks seen so far in this file
        for i, m in enumerate(CPP_BLOCK.finditer(text)):
            code = m.group(1)
            first = code.lstrip().split("\n", 1)[0]
            if "nocheck" in first or "sketch" in first:
                continue
            total += 1
            frag = "frag" in first
            body = ("void __frag(){\n" + code + "\n}\n") if frag else code
            has_main = "main(" in code
            tail = "" if has_main else "\nint main(){}\n"
            if "#include <bits/stdc++.h>" in code:
                ok, err = compile_ok(gxx, code + tail, tmpdir)
            else:
                ok, err = compile_ok(gxx, PRELUDE + body + tail, tmpdir)
                if not ok and earlier:
                    ok, err = compile_ok(gxx, PRELUDE + "\n".join(earlier) + body + tail, tmpdir)
            if not frag and not has_main and "#include <bits/stdc++.h>" not in code:
                earlier.append(code)
            line = text[:m.start()].count("\n") + 1
            if not ok:
                bad += 1
                print(f"--- ERROR: {f.relative_to(ROOT)}:{line} (block {i})")
                print("\n".join(err.splitlines()[:12]))
            elif err.strip():
                print(f"--- warn: {f.relative_to(ROOT)}:{line} (block {i})")
                print("\n".join(err.splitlines()[:6]))
    print(f"checked {total} blocks, {bad} failed")
    sys.exit(1 if bad else 0)


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--html", action="store_true")
    a = ap.parse_args()
    if a.check:
        check()
    else:
        build_pdf(a.html)
