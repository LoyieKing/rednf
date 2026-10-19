#!/usr/bin/env python3
# Print relocations in [start,stop): vt.py <hexstart> <hexstop>
import subprocess, sys
L = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394"
start = int(sys.argv[1], 16); stop = int(sys.argv[2], 16)
out = subprocess.run(["readelf", "-rW", L], capture_output=True, text=True).stdout
rel = {}
for line in out.splitlines():
    p = line.split()
    if len(p) < 5 or not p[0].lower().startswith("0"):
        continue
    try:
        a = int(p[0], 16)
    except ValueError:
        continue
    if start <= a < stop:
        # fields: offset info type symvalue symname
        rel[a] = (p[2], p[4] if len(p) > 4 else "")
ad = subprocess.run(["objdump", "-s", "--start-address=%d" % start,
                     "--stop-address=%d" % stop, L], capture_output=True, text=True).stdout
for line in ad.splitlines():
    if "  " in line and line.strip() and line.strip()[0] in "0123456789abcdef":
        parts = line.split()
        if len(parts) < 2:
            continue
        try:
            base = int(parts[0], 16)
        except ValueError:
            continue
        hexs = "".join(parts[1:5])
        for i in range(0, min(len(hexs), 16), 4):
            a = base + i
            val = hexs[i:i+8]
            sym = rel.get(a)
            print("%08x  %s  %s" % (a, val, ("%s %s" % sym) if sym else ""))
