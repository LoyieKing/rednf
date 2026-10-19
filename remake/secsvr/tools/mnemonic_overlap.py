#!/usr/bin/env python3
"""函数级助记符重叠：我们的 .o 与二进制逐符号比对归一化助记符序列。

用法：python3 mnemonic_overlap.py <binary> <object.o> [--min N]
输出每符号 mnemonics 重叠率（SequenceMatcher），低于 --min（默认 0.85）标 WARN。
归一化：保留助记符 + 操作数类别（reg/imm/addr/mem），去掉具体寄存器与立即数。
"""
import re
import subprocess
import sys
from difflib import SequenceMatcher


def disasm(path, start=None, stop=None, sym=None):
    cmd = ["objdump", "-d", "--no-show-raw-insn"]
    if sym is not None:
        cmd += ["--disassemble=" + sym]
    elif start is not None:
        cmd += [f"--start-address={start:#x}", f"--stop-address={stop:#x}"]
    cmd.append(path)
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode != 0:
        return []
    out = []
    for line in r.stdout.splitlines():
        line = line.strip()
        if not line or "\t" not in line:
            continue
        # objdump -d 格式: addr: 助记符 操作数
        m = re.match(r"^[0-9a-f]+:\s+(.*)$", line)
        if not m:
            continue
        insn = m.group(1)
        if insn.startswith("(bad)"):
            continue
        out.append(insn)
    return out


def normalize(insn):
    """助记符 + 操作数类别化。"""
    parts = insn.split(None, 1)
    mnem = parts[0]
    ops = parts[1] if len(parts) > 1 else ""
    norm = []
    for tok in re.split(r"[,\s]+", ops.strip()):
        if not tok:
            continue
        if tok.startswith("0x") or re.match(r"^-?\d+$", tok):
            norm.append("imm")
        elif re.match(r"^[re][abcdsdi][xpl]?$|^e(ax|bx|cx|dx|si|di|bp|sp)$", tok) or \
                re.match(r"^[abcdsdi][xpl]$|^bp$|^sp$|^si$|^di$", tok):
            norm.append("reg")
        elif re.match(r"^[abcdsdi][xpl]$", tok):
            norm.append("reg")
        elif "(" in tok:  # 内存寻址
            norm.append("mem")
        elif tok.startswith("*"):
            norm.append("mem")
        else:
            norm.append("?")
    return mnem + " " + " ".join(norm)


def syms_in_obj(obj):
    """返回 {demangled: mangled}。"""
    r = subprocess.run(["nm", obj], capture_output=True, text=True).stdout
    out = {}
    mangled = None
    for line in r.splitlines():
        parts = line.split(None, 2)
        if len(parts) < 3:
            continue
        addr, kind, name = parts
        if kind in ("T", "t", "W"):
            mangled = name
        else:
            continue
        if re.match(r"^(__tcf_|_GLOBAL__|__static_initialization|global constructors|global destructors)", mangled):
            continue
        dm = subprocess.run(["c++filt", mangled], capture_output=True, text=True).stdout.strip()
        out[dm] = mangled
    return out


def sym_addr(binary, demangled):
    r = subprocess.run(["nm", "-C", binary], capture_output=True, text=True).stdout
    for line in r.splitlines():
        parts = line.split(None, 2)
        if len(parts) >= 3 and parts[2] == demangled and parts[1] in ("T", "t", "W"):
            return int(parts[0], 16)
    return None


def size_of(binary, addr):
    """下一个符号地址减当前地址作为函数大小。"""
    r = subprocess.run(["nm", binary], capture_output=True, text=True).stdout
    addrs = []
    for line in r.splitlines():
        parts = line.split(None, 2)
        if len(parts) >= 2 and parts[1] in ("T", "t", "W") and re.match(r"^[0-9a-f]+$", parts[0]):
            addrs.append(int(parts[0], 16))
    addrs.sort()
    for i, a in enumerate(addrs):
        if a == addr:
            return (addrs[i + 1] - a) if i + 1 < len(addrs) else 0x40
    return 0x40


def main():
    binary, obj = sys.argv[1], sys.argv[2]
    mn = 0.85
    if "--min" in sys.argv:
        mn = float(sys.argv[sys.argv.index("--min") + 1])
    ours = syms_in_obj(obj)
    checked = 0
    low = []
    for s in sorted(ours):
        mangled = ours[s]
        addr = sym_addr(binary, s)
        if addr is None:
            continue
        size = size_of(binary, addr)
        a = disasm(obj, sym=mangled)
        b = disasm(binary, start=addr, stop=addr + size)
        na = [normalize(x) for x in a]
        nb = [normalize(x) for x in b]
        if not na or not nb:
            continue
        ratio = SequenceMatcher(None, na, nb).ratio()
        checked += 1
        flag = "WARN" if ratio < mn else ""
        if ratio < mn:
            low.append((s, ratio))
        print(f"{ratio:5.3f} {flag:4s} {s}")
    print(f"-- 检查 {checked} 个函数；低重叠 {len(low)} 个")
    for s, r in sorted(low, key=lambda x: x[1]):
        print(f"  LOW {r:.3f} {s}")


if __name__ == "__main__":
    main()
