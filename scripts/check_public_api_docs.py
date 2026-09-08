#!/usr/bin/env python3
"""Fail if public C++ headers under include/ir/ appear to lack documentation.

Rules (lightweight, pragmatic):
- Skip empty files and files with no class/struct/function declarations.
- If a file declares a class/struct or a function-like declaration, require at
  least one Doxygen-style marker (/// or /**) in the file.
- This does not validate full @param/@return quality; it only checks presence
  of documentation comments once API surface exists.

Exit codes:
  0 — OK
  1 — documentation gaps found
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
INCLUDE_IR = ROOT / "include" / "ir"

DECL_RE = re.compile(
    r"\b(class|struct)\s+\w+"
    r"|\b[\w:<>*&]+(?:\s+|\s*\*+\s*|\s*&\s*)+\w+\s*\([^;]*\)\s*(const)?\s*(noexcept)?\s*(=\s*0)?\s*[;{]"
)
DOC_RE = re.compile(r"///|/\*\*")


def header_files() -> list[Path]:
    if not INCLUDE_IR.is_dir():
        return []
    return sorted(INCLUDE_IR.rglob("*.hpp")) + sorted(INCLUDE_IR.rglob("*.h"))


def analyze(path: Path) -> str | None:
    text = path.read_text(encoding="utf-8", errors="replace")
    if not text.strip():
        return None  # empty placeholder headers are OK for now

    if not DECL_RE.search(text):
        return None  # no API surface yet

    if not DOC_RE.search(text):
        rel = path.relative_to(ROOT).as_posix()
        return f"{rel}: public declarations found but no Doxygen-style comments (/// or /**)"

    return None


def main() -> int:
    problems = []
    for path in header_files():
        issue = analyze(path)
        if issue:
            problems.append(issue)

    if problems:
        print("Public API documentation check failed:\n")
        for p in problems:
            print(f"  - {p}")
        print(
            "\nSee docs/coding-standards.md — public API must document purpose, "
            "parameters, and return values."
        )
        return 1

    count = len(header_files())
    print(f"Public API documentation check passed ({count} header(s) under include/ir/).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
