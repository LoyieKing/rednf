#!/usr/bin/env python3
"""行为对照：我们的库 vs secagent 二进制（通过 gdb call 调用 Ten* 函数）。

用法：python3 behavior_check.py <Class> <Method> <key_hex> <data_hex>
例：python3 behavior_check.py CIdea Idea 000102030405060708090a0b0c0d0e0f 0011223344556677
    （二进制侧调用 mangled 类方法，本侧构造对象调用同方法）

输出：OUR=... BIN=... 一致则 PASS。
"""
import subprocess
import sys
import os

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

CLASS_HEADER = {
    "CTean": "tean.h", "CIdea": "idea.h", "CRc4": "rc4.h",
    "CTwofish": "twofish.h", "CBlowFish": "blowfish.h", "CRijndael": "rijndael.h",
    "CGost": "gost.h", "CCast": "cast.h", "CDES": "des.h",
    "CThreeWay": "3way.h", "CDiamond": "diamond.h", "CNsea": "nsea.h",
    "CHpc": "hpc.h", "CMmb": "mmb.h", "CMars": "mars.h",
    "CSafer": "safer.h", "CSquare": "square.h", "CFrog": "frog.h",
    "CFeal8": "feal8.h", "CLoki89": "loki89.h", "CLoki91": "loki91.h",
    "CLoki97": "loki97.h", "CRc2": "rc2.h", "CRc5": "rc5.h",
    "CRc6": "rc6.h", "CMagic8": "magic.h", "CMbc": "mbc.h",
    "CAdder": "adder.h", "CSubtracter": "subtracter.h", "CAmoeba": "amoeba.h",
    "CRing": "ring.h", "CShift": "shift.h", "CGramary": "gramary.h",
    "CGene": "gene.h", "CSeattos": "seattos.h", "CD3des": "d3des.h",
    "CDes2p": "des2p.h", "CDesBig": "desbig.h", "CDesMo": "desmo.h",
    "CDesNew": "desnew.h", "CDesPC": "despc.h", "CDesToo": "destoo.h",
    "CFastDes": "fastdes.h",
}


def hex_to_bytes(h):
    return bytes.fromhex(h)


def run_ours(cls, method, key, data):
    header = CLASS_HEADER.get(cls, cls.lower() + ".h")
    cpp = f'''
#include <stdio.h>
#include <string.h>
#include "inc/{header}"
#include "include/TenCrypt.h"
struct StubLocal : ITsLocal {{
    void ProcessLastBytes(unsigned char*, bool, unsigned int, unsigned int, unsigned char*) {{}}
    void * LocalGetModuleHandle() {{ return 0; }}
    void * LocalGetProcAddress() {{ return 0; }}
    void Seattos(unsigned char*, bool, unsigned char*, unsigned int) {{}}
}};
static StubLocal g_stub;
ITsLocal * CreateTsLocal() {{ return &g_stub; }}
void FreeTsLocal() {{}}
int main() {{
    unsigned char key[32] = {{0}};
    unsigned char buf[128] = {{0}};
    int kl = {len(key)}, dl = {len(data)};
    unsigned char k[]{'{'+','.join('0x%02x' % b for b in key)+'}'};
    unsigned char d[]{'{'+','.join('0x%02x' % b for b in data)+'}'};
    memcpy(key, k, kl);
    memcpy(buf, d, dl);
    {cls} obj;
    obj.{method}(key, 1, buf, dl);
    for (int i = 0; i < dl; i++) printf("%02x", buf[i]);
    printf("\\n");
    return 0;
}}
'''
    path = os.path.join("/tmp", "bc_test.cpp")
    with open(path, "w") as f:
        f.write(cpp)
    lib = os.path.join(ROOT, "tencrypt_new", "libtencrypt_new.a")
    subprocess.run(["g++", "-m32", "-O0", "-I", os.path.join(ROOT, "tencrypt_new"),
                    path, lib, "-o", "/tmp/bc_test"],
                   check=True, capture_output=True)
    out = subprocess.run(["/tmp/bc_test"], capture_output=True, text=True).stdout.strip()
    return out


def run_bin(cls, method, key, data):
    khex = "".join("%02x" % b for b in key)
    dhex = "".join("%02x" % b for b in data)
    dl = len(data)
    gdb_cmds = [
        "file " + BIN,
        "starti",
        "set $k = (unsigned char*)malloc(256)",
        "set $b = (unsigned char*)malloc(256)",
        "set {unsigned char[256]}$k = {" + ",".join(str(b) for b in key) + "}",
        "set {unsigned char[256]}$b = {" + ",".join(str(b) for b in data) + "}",
        f"call (void){cls}::{method}($k, 1, $b, {dl})",
        f"x/{dl}bx $b",
        "quit",
    ]
    script = "\n".join(gdb_cmds)
    out = subprocess.run(["gdb", "-batch", "-ex", script], capture_output=True,
                         text=True, timeout=120).stdout
    # 解析 x/ 输出：地址行后跟 hex 字节
    lines = out.splitlines()
    res = []
    for line in lines:
        parts = line.split()
        for p in parts:
            if len(p) == 2 and all(c in "0123456789abcdefABCDEF" for c in p):
                res.append(p.lower())
    return "".join(res[:len(data)])


def main():
    cls = sys.argv[1]
    method = sys.argv[2]
    key = hex_to_bytes(sys.argv[3])
    data = hex_to_bytes(sys.argv[4])
    ours = run_ours(cls, method, key, data)
    try:
        bins = run_bin(cls, method, key, data)
    except subprocess.TimeoutExpired:
        bins = "TIMEOUT"
    print(f"OUR = {ours}")
    print(f"BIN = {bins}")
    print("PASS" if ours == bins else "FAIL")


if __name__ == "__main__":
    main()
