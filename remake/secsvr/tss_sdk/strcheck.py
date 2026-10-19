#!/usr/bin/env python3
"""Check that each UTF-8 literal from argv exists byte-exactly in a binary."""
import sys

data = open(sys.argv[1], "rb").read()
ok = True
for needle in sys.argv[2:]:
    b = needle.encode("utf-8")
    if b in data:
        print("PRESENT  %r" % needle)
    else:
        ok = False
        print("MISSING  %r" % needle)
sys.exit(0 if ok else 1)
