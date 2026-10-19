#!/usr/bin/env python3
"""Byte-level comparison of the reconstructed formmog_proto_antibot_client
against the secagent binary.

My side: a 64-bit host harness compiled from the generated sources.
Binary side: uni_call (32-bit emulation) invoking the same methods.
"""
from __future__ import print_function

import os
import random
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "toolchain"))
import gen_formmog_antibot as G

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"
HARNESS = "/tmp/verify_antibot_harness"
OBJ = 0x00820000
OBJ_BASE = 0x00820000
WIRE_BUF_BASE = 0x00600000
DATA_END = 0x00610000
USED = 0x00800000
TWBUF = 0x00800100
TW_DATA = 0x00810000


def cls_size(name):
    return G.classes[name]["size"]


def field_ofs(cls, name):
    if "." in name:
        head, rest = name.split(".", 1)
        sub = None
        for m in G.classes[cls]["members"]:
            if m["name"] == head:
                sub = m["type"]
                break
        if sub is None:
            raise KeyError(name)
        return field_ofs(cls, head) + field_ofs(sub, rest)
    for m in G.classes[cls]["members"]:
        if m["name"] == name:
            return m["off"]
    raise KeyError(name)


def build_object(cls, rng, depth=0):
    """Deterministic pseudo-random but schema-valid object bytes."""
    size = cls_size(cls)
    buf = bytearray(rng.getrandbits(8) for _ in range(size))
    if cls in G.UNION_CLASSES:
        # union: choose the first member and fill it (zero the rest)
        mname, typ = G.UNION_MEMBERS[cls][0]
        if typ in G.classes:
            sub = build_object(typ, rng, depth + 1)
            buf[:len(sub)] = sub
        else:
            buf[:4] = rng.getrandbits(32).to_bytes(4, "little")
        return bytes(buf)
    if depth > 3:
        return bytes(buf)
    fields = G.classify(cls, G.classes[cls]["members"], G.classes)
    pairs = dict((v, k) for k, v in G.VAR_PAIRS.get(cls, []))
    for f in fields:
        kind = f[0]
        off = field_ofs(cls, f[1])
        if kind == "scalar":
            pass  # already random
        elif kind == "str":
            s = rng.choice([b"", b"a", b"hello", b"x" * 7, "中文测试".encode("utf-8")])
            n = min(len(s), f[2] - 1)
            buf[off:off + n] = s[:n]
            buf[off + f[2] - 1] = 0
        elif kind == "buf":
            cnt = f[3]
            maxn = f[2]
            n = rng.randint(0, min(8, maxn))
            cof = field_ofs(cls, cnt)
            cntkind = f[4]
            if cntkind == "u8":
                buf[cof] = n
            elif cntkind == "u16":
                buf[cof:cof + 2] = n.to_bytes(2, "little")
            else:
                buf[cof:cof + 4] = n.to_bytes(4, "little")
            for i in range(n):
                buf[off + i] = rng.getrandbits(8)
        elif kind == "typedarr":
            cnt = f[4]
            esize = 2 if f[2] == "uint16_t" else 4
            maxn = f[3] // esize
            n = rng.randint(0, min(5, maxn))
            cof = field_ofs(cls, cnt)
            ck = f[5]
            if ck == "u8":
                buf[cof] = n
            elif ck == "u16":
                buf[cof:cof + 2] = n.to_bytes(2, "little")
            else:
                buf[cof:cof + 4] = n.to_bytes(4, "little")
            for i in range(n):
                buf[off + i * esize:off + (i + 1) * esize] = rng.getrandbits(8 * esize).to_bytes(esize, "little")
        elif kind == "objarr":
            cnt = f[4]
            mx = f[3] // cls_size(f[2])
            n = rng.randint(0, min(3, mx))
            cof = field_ofs(cls, cnt)
            ck = f[5]
            if ck == "u8":
                buf[cof] = n
            elif ck == "u16":
                buf[cof:cof + 2] = n.to_bytes(2, "little")
            else:
                buf[cof:cof + 4] = n.to_bytes(4, "little")
            for i in range(n):
                sub = build_object(f[2], rng, depth + 1)
                buf[off + i * cls_size(f[2]):off + (i + 1) * cls_size(f[2])] = sub
        elif kind == "obj":
            sub = build_object(f[2], rng, depth + 1)
            buf[off:off + cls_size(f[2])] = sub
    return bytes(buf)


def harness(cls, op, selector, hexin):
    p = subprocess.run([HARNESS, cls, op, str(selector), hexin],
                       capture_output=True, text=True)
    if p.returncode != 0:
        raise RuntimeError("harness failed %s %s: %s" % (cls, op, p.stderr))
    m = re.search(r"OUT (\w+) (\d+) ([0-9a-f]+)", p.stdout)
    if not m:
        raise RuntimeError("bad harness out: %s" % p.stdout)
    return m.group(1), int(m.group(2)), m.group(3)


def nm_syms():
    out = subprocess.run(["nm", "-n", BIN], capture_output=True, text=True).stdout
    syms = {}
    for ln in out.splitlines():
        p = ln.split()
        if len(p) >= 3:
            syms[" ".join(p[2:])] = int(p[0], 16)
    return syms


def bin_call(args, reads=()):
    """Run uni_call and return (printed buffer hex lines, read dict)."""
    p = subprocess.run(
        [sys.executable, os.path.join(ROOT, "toolchain/uni_call.py"),
         "--bin", BIN, "--plain", "--ret"] + args,
        capture_output=True, text=True, cwd=ROOT)
    if p.returncode != 0:
        raise RuntimeError("uni_call failed: %s" % p.stderr[-500:])
    bufs = []
    rd = {}
    for ln in p.stdout.splitlines():
        if ln.startswith("READ "):
            parts = ln.split()
            rd[parts[1]] = parts[2]
        elif re.fullmatch(r"[0-9a-f]+", ln.strip()):
            bufs.append(ln.strip())
        elif ln.startswith("RET="):
            rd["RET"] = ln
    return bufs, rd


def bin_pack(mangled, objhex, buflen):
    buf = "00" * buflen
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, objhex), "0x%x" % obj_addr, buf,
            "int:%d" % buflen, "0x0", "int:0"]
    bufs, rd = bin_call([mangled] + args)
    return bufs[0], rd


def bin_pack_union(mangled, selector, objhex, buflen):
    buf = "00" * buflen
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, objhex), "0x%x" % obj_addr,
            "int:%d" % (selector & 0xFFFFFFFF), "int:%d" % ((selector >> 32) & 0xFFFFFFFF),
            buf, "int:%d" % buflen, "0x0", "int:0"]
    bufs, rd = bin_call([mangled] + args)
    return bufs[0], rd


def bin_unpack(mangled, wirehex, objsize):
    buflen = len(wirehex) // 2
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, "00" * 256), "0x%x" % obj_addr, wirehex,
            "int:%d" % buflen, "0x0", "int:0", "r:%x:%x" % (obj_addr, objsize)]
    bufs, rd = bin_call([mangled] + args)
    return rd["%x" % obj_addr]


def bin_unpack_union(mangled, selector, wirehex, objsize):
    buflen = len(wirehex) // 2
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, "00" * 256), "0x%x" % obj_addr,
            "int:%d" % (selector & 0xFFFFFFFF), "int:%d" % ((selector >> 32) & 0xFFFFFFFF),
            wirehex, "int:%d" % buflen, "0x0", "int:0",
            "r:%x:%x" % (obj_addr, objsize)]
    bufs, rd = bin_call([mangled] + args)
    return rd["%x" % obj_addr]


def bin_visualize(mangled, objhex, buflen=8192):
    buf = "00" * buflen
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, objhex), "0x%x" % obj_addr, buf,
            "int:%d" % buflen, "0x0", "int:1", "int:44"]
    bufs, rd = bin_call([mangled] + args)
    return bufs[0], rd


def bin_visualize_union(mangled, selector, objhex, buflen=8192):
    buf = "00" * buflen
    off = buflen + 16
    obj_addr = OBJ_BASE
    args = ["w:%x:%s" % (obj_addr, objhex), "0x%x" % obj_addr,
            "int:%d" % (selector & 0xFFFFFFFF), "int:%d" % ((selector >> 32) & 0xFFFFFFFF),
            buf, "int:%d" % buflen, "0x0", "int:1", "int:44"]
    bufs, rd = bin_call([mangled] + args)
    return bufs[0], rd


def bin_construct(mangled, objsize):
    args = ["w:%x:%s" % (OBJ, "55" * objsize), "0x%x" % OBJ,
            "r:%x:%x" % (OBJ, objsize)]
    bufs, rd = bin_call([mangled] + args)
    return rd["%x" % OBJ]


def bin_visualize_ex(mangled, objhex, used, limit=8192):
    off = 0
    obj_addr = OBJ_BASE
    tw = "w:%x:%s" % (TWBUF, "%08x" % TW_DATA + "00000000" + "%08x" % limit + "00000000")
    args = [tw, "w:%x:%s" % (obj_addr, objhex), "0x%x" % obj_addr,
            "0x%x" % TWBUF, "int:1", "int:44", "r:%x:%x" % (TW_DATA, used)]
    bufs, rd = bin_call([mangled] + args)
    return rd["%x" % TW_DATA]


def main():
    G.classes = G.get_classes()
    syms = nm_syms()
    rng = random.Random(12345)
    order = G.topo_order(G.classes)
    regular = [c for c in order if c not in G.UNION_CLASSES]
    only = set(sys.argv[1:])
    if only:
        regular = [c for c in regular if c in only]
    fails = []
    total = 0

    def check(tag, cls, mine_hex, bin_hex):
        nonlocal total
        total += 1
        if mine_hex != bin_hex:
            fails.append((tag, cls, mine_hex[:80], bin_hex[:80]))
            print("FAIL %s %s\n  mine=%s...\n  bin =%s..." % (tag, cls, mine_hex[:80], bin_hex[:80]))
        else:
            print("ok   %s %s (%d bytes)" % (tag, cls, len(mine_hex) // 2))

    # ---- construct
    for cls in regular:
        size = cls_size(cls)
        mangled = "_ZN9sec_proto%d%s9constructEv" % (len(cls), cls)
        m, n, mhex = harness(cls, "construct", 0, "55" * size)
        bhex = bin_construct(mangled, size)
        check("construct", cls, mhex, bhex)

    # ---- pack / unpack / visualize with random objects (one round)
    for cls in regular:
        size = cls_size(cls)
        obj = build_object(cls, rng)
        objhex = obj.hex()
        m, n, wirehex = harness(cls, "pack", 0, objhex)
        packm = "_ZNK9sec_proto%d%s4packEPcjPjj" % (len(cls), cls)
        bwire, _ = bin_pack(packm, objhex, 16384)
        check("pack", cls, wirehex, bwire[:len(wirehex)])
        # unpack round trip on binary side
        unpackm = "_ZN9sec_proto%d%s6unpackEPKcjPjj" % (len(cls), cls)
        m, n, mine_obj = harness(cls, "unpack", 0, wirehex)
        bobj = bin_unpack(unpackm, wirehex, size)
        check("unpack", cls, mine_obj, bobj[:len(mine_obj)])
        # visualize
        vizm = "_ZNK9sec_proto%d%s9visualizeEPcjPjic" % (len(cls), cls)
        m, n, vhex = harness(cls, "visualize", 0, objhex)
        bv, _ = bin_visualize(vizm, objhex)
        check("visualize", cls, vhex, bv[:len(vhex)])

    # ---- union classes: each dispatch case, pack/unpack/visualize/construct
    for cls in G.UNION_CLASSES:
        size = cls_size(cls)
        cases = sorted(G.UNION_DISPATCH[cls].keys())
        if cls == "SAB_CS_SLN_DATA":
            cases = [1, 2, 19, 100, 201, 302, 601]  # representative incl. range
        elif cls == "AntibotPkgBody":
            cases = [1, 6, 8, 27, 48, 62, 70, 100, 120, 121]
        for case in cases:
            obj = build_object(cls, rng)
            objhex = obj.hex()
            m, n, wirehex = harness(cls, "pack", case, objhex)
            packm = "_ZNK9sec_proto%d%s4packExPcjPjj" % (len(cls), cls)
            bwire, _ = bin_pack_union(packm, case, objhex, 16384)
            check("pack", "%s[%d]" % (cls, case), wirehex, bwire[:len(wirehex)])
            unpackm = "_ZN9sec_proto%d%s6unpackExPKcjPjj" % (len(cls), cls)
            m, n, mine_obj = harness(cls, "unpack", case, wirehex)
            bobj = bin_unpack_union(unpackm, case, wirehex, size)
            check("unpack", "%s[%d]" % (cls, case), mine_obj, bobj[:len(mine_obj)])
            vizm = "_ZNK9sec_proto%d%s9visualizeExPcjPjic" % (len(cls), cls)
            m, n, vhex = harness(cls, "visualize", case, objhex)
            bv, _ = bin_visualize_union(vizm, case, objhex)
            check("visualize", "%s[%d]" % (cls, case), vhex, bv[:len(vhex)])
        # construct
        ctor = "_ZN9sec_proto%d%s9constructEx" % (len(cls), cls)
        for case in cases[:3]:
            m, n, mhex = harness(cls, "construct", case, "55" * size)
            args = ["w:%x:%s" % (OBJ, "55" * size), "0x%x" % OBJ,
                    "int:%d" % (case & 0xFFFFFFFF), "int:%d" % ((case >> 32) & 0xFFFFFFFF),
                    "r:%x:%x" % (OBJ, size)]
            bufs, rd = bin_call([ctor] + args)
            check("construct", "%s[%d]" % (cls, case), mhex, rd["%x" % OBJ][:len(mhex)])

    # ---- parent dispatch classes with a valid selector
    sel_cases = {
        "AntibotClientPkg": ("body_", "head_.cmd_id_", 27),
        "TAB_SC_SCHEMEDATA": ("data_", "sub_protocol_id_", 105),
        "TAB_CS_SCHEMEDATA": ("data_", "sub_protocol_id_", 19),
        "TSANLIX_CS_REPORT": ("trojan_", "type_", 1),
        "TMEM_RECOVER_ITEM": ("name_or_ordinal_", "id_", 1),
    }
    for cls, (member, sel, case) in sel_cases.items():
        size = cls_size(cls)
        obj = build_object(cls, rng)
        seloff = field_ofs(cls, sel)
        selkind = [m["type"] for m in G.classes[cls]["members"] if m["name"] == sel][0]
        if selkind == "uint8_t":
            obj = obj[:seloff] + bytes([case]) + obj[seloff + 1:]
        elif selkind == "uint16_t":
            obj = obj[:seloff] + case.to_bytes(2, "little") + obj[seloff + 2:]
        else:
            obj = obj[:seloff] + case.to_bytes(4, "little") + obj[seloff + 4:]
        objhex = obj.hex()
        m, n, wirehex = harness(cls, "pack", 0, objhex)
        packm = "_ZNK9sec_proto%d%s4packEPcjPjj" % (len(cls), cls)
        bwire, _ = bin_pack(packm, objhex, 16384)
        check("pack", cls + "@sel", wirehex, bwire[:len(wirehex)])
        unpackm = "_ZN9sec_proto%d%s6unpackEPKcjPjj" % (len(cls), cls)
        m, n, mine_obj = harness(cls, "unpack", 0, wirehex)
        bobj = bin_unpack(unpackm, wirehex, size)
        check("unpack", cls + "@sel", mine_obj, bobj[:len(mine_obj)])

    print()
    print("total=%d fails=%d" % (total, len(fails)))
    if fails:
        print("FAILED CASES:")
        for f in fails[:40]:
            print(" ", f)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
