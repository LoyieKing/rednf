#!/usr/bin/env python3
"""Generate a 64-bit host harness for formmog_proto_antibot_client.

The harness exposes per-class pack/unpack/visualize/visualize_ex/construct
entry points so the python driver can compare byte-for-byte against the
secagent binary via uni_call.
"""
from __future__ import print_function

import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "toolchain"))
import gen_formmog_antibot as G


HARNESS_HEAD = (
    "// Auto-generated verification harness (64-bit host build).\n"
    '#include "src/protocol/formmog/formmog_proto_antibot_client.h"\n'
    '#include "src/protocol/common/TdrBuf.h"\n'
    '#include "src/protocol/common/TdrBufUtil.h"\n'
    "#include <cstdio>\n"
    "#include <cstdlib>\n"
    "#include <cstring>\n"
    "#include <string>\n\n"
    "using namespace sec_proto;\n"
    "using namespace tsf4g_tdr;\n\n"
    "static unsigned char obj[131072];\n"
    "static unsigned char wire[262144];\n"
    "static unsigned char text[262144];\n\n"
    "static int hexval(char c) {\n"
    "    if (c >= '0' && c <= '9') return c - '0';\n"
    "    if (c >= 'a' && c <= 'f') return c - 'a' + 10;\n"
    "    if (c >= 'A' && c <= 'F') return c - 'A' + 10;\n"
    "    return -1;\n"
    "}\n\n"
    "static size_t hexdecode(const char* s, unsigned char* out) {\n"
    "    size_t n = 0;\n"
    "    while (s[0] && s[1]) {\n"
    "        int hi = hexval(s[0]);\n"
    "        int lo = hexval(s[1]);\n"
    "        if (hi < 0 || lo < 0) break;\n"
    "        out[n++] = (unsigned char)((hi << 4) | lo);\n"
    "        s += 2;\n"
    "    }\n"
    "    return n;\n"
    "}\n\n"
    "static void print_hex(const unsigned char* p, size_t n) {\n"
    "    for (size_t i = 0; i < n; ++i) printf(\"%02x\", p[i]);\n"
    "    printf(\"\\n\");\n"
    "}\n\n"
    "static void hexout(const char* tag, const unsigned char* p, size_t n) {\n"
    "    printf(\"OUT %s %zu \", tag, n);\n"
    "    print_hex(p, n);\n"
    "}\n\n"
)


def gen_dispatch(classes, order):
    lines = []
    for cname in order:
        if cname in G.UNION_CLASSES:
            lines.append('    if (strcmp(cls, "%s") == 0) {' % cname)
            lines.append("        %s* o = (%s*)obj;" % (cname, cname))
            lines.append("        long long sel = selector;")
            lines.append('        if (strcmp(op, "construct") == 0) { o->construct(sel); hexout("obj", obj, sizeof(%s)); }' % cname)
            lines.append('        else if (strcmp(op, "pack") == 0) { TdrWriteBuf b((char*)wire, sizeof wire); o->pack(sel, b, 0); hexout("wire", wire, b.size()); }')
            lines.append('        else if (strcmp(op, "unpack") == 0) { TdrReadBuf b((const char*)wire, wire_len); o->unpack(sel, b, 0); hexout("obj", obj, sizeof(%s)); }' % cname)
            lines.append('        else if (strcmp(op, "visualize") == 0) { TdrWriteBuf b((char*)text, sizeof text); o->visualize(sel, b, indent, sep); hexout("text", text, b.size()); }')
            lines.append('        else { fprintf(stderr, "bad op %s\\n", op); return 2; }')
            lines.append("        return 0;")
            lines.append("    }")
        else:
            lines.append('    if (strcmp(cls, "%s") == 0) {' % cname)
            lines.append("        %s* o = (%s*)obj;" % (cname, cname))
            lines.append('        if (strcmp(op, "construct") == 0) { o->construct(); hexout("obj", obj, sizeof(%s)); }' % cname)
            lines.append('        else if (strcmp(op, "pack") == 0) { TdrWriteBuf b((char*)wire, sizeof wire); o->pack(b, 0); hexout("wire", wire, b.size()); }')
            lines.append('        else if (strcmp(op, "unpack") == 0) { TdrReadBuf b((const char*)wire, wire_len); o->unpack(b, 0); hexout("obj", obj, sizeof(%s)); }' % cname)
            lines.append('        else if (strcmp(op, "visualize") == 0) { TdrWriteBuf b((char*)text, sizeof text); o->visualize(b, indent, sep); hexout("text", text, b.size()); }')
            lines.append('        else if (strcmp(op, "visualize_ex") == 0) { TdrWriteBuf b((char*)text, sizeof text); o->visualize_ex(b, indent, sep); hexout("text", text, b.size()); }')
            lines.append('        else { fprintf(stderr, "bad op %s\\n", op); return 2; }')
            lines.append("        return 0;")
            lines.append("    }")
    return "\n".join(lines)


def main():
    G.classes = G.get_classes()
    order = G.topo_order(G.classes)
    body = HARNESS_HEAD
    body += "int main(int argc, char** argv) {\n"
    body += '    if (argc < 5) { fprintf(stderr, "usage: harness CLASS OP SELECTOR HEXIN\\n"); return 2; }\n'
    body += "    const char* cls = argv[1];\n"
    body += "    const char* op = argv[2];\n"
    body += "    long long selector = atoll(argv[3]);\n"
    body += "    int indent = 1;\n"
    body += "    char sep = ',';\n"
    body += "    size_t wire_len = hexdecode(argv[4], wire);\n"
    body += "    memset(obj, 0, sizeof obj);\n"
    body += '    if (strcmp(op, "pack") == 0 || strcmp(op, "visualize") == 0 || strcmp(op, "visualize_ex") == 0 || strcmp(op, "construct") == 0) { hexdecode(argv[4], obj); }\n'
    body += gen_dispatch(G.classes, order)
    body += '    fprintf(stderr, "unknown class %s\\n", cls);\n'
    body += "    return 1;\n"
    body += "}\n"
    out = "/tmp/verify_antibot_harness.cpp"
    open(out, "w").write(body)
    print(out)


if __name__ == "__main__":
    main()
