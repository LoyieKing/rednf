#!/bin/bash
# Compare ORIG vs recon call/string sequences for a CfgSvrSdk method.
# usage: diffcalls.sh <symbol-substring> <orig-start-hex> <orig-size-hex>
L=/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394
O=/tmp/cfg.o
SYM="$1"; START="$2"; SIZE="$3"
python3 - "$SYM" "$START" "$SIZE" <<'PY'
import subprocess, sys, re
O = "/tmp/cfg.o"
sym, start, size = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
L = "/opt/dnf1031/home/template/neople/game/libtss_sdk.so.1.0.394"

def calls(path, lo, hi, name_filter=None):
    out = subprocess.run(["objdump", "-d", "-C", "--start-address=%d" % lo,
                          "--stop-address=%d" % hi, path],
                         capture_output=True, text=True).stdout
    res = []
    for line in out.splitlines():
        m = re.search(r"call\s+[0-9a-f]+ <([^>]+)>", line)
        if m:
            res.append(m.group(1))
    return res

print("== ORIG ==  (%d bytes from 0x%x)" % (size, start))
for c in calls(L, start, start + size):
    print("   ", c)

out = subprocess.run(["nm", "-C", "--defined-only", O], capture_output=True, text=True).stdout
lo = hi = None
for line in out.splitlines():
    if sym in line and " t " not in line[:8]:
        parts = line.split()
        lo = int(parts[0], 16)
        break
if lo is None:
    print("symbol not found in recon object"); sys.exit()
# find next symbol address to bound the function
addrs = sorted(int(l.split()[0], 16) for l in out.splitlines()
               if l.split() and re.match(r"^[0-9a-f]+$", l.split()[0]))
hi = next((a for a in addrs if a > lo), lo + 4096)
print("== RECON == (%d bytes from 0x%x)" % (hi - lo, lo))
for c in calls(O, lo, hi):
    print("   ", c)
PY
