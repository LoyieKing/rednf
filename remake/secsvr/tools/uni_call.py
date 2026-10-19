#!/usr/bin/env python3
"""unicorn 调用器：模拟执行 secagent 代码，调用任意符号函数。

用法：python3 uni_call.py [--bin <binary>] [--plain] [--ret] <symbol> <arg>...
  --bin 默认 secagent；可用 gunnersvr / zergsvr 路径（三件套共享同一套调用约定）。
  --plain 关闭 _ZN 的自动 this 构造（用于命名空间作用域函数，如 ZEN_OS::*、tsf4g_tdr::*）。
  --ret 额外打印 EAX（整数返回值；64 位返回请配合看 EDX）。
  数据参数：hex 字符串（分配 guest 内存传指针，调用后读回）
  整数参数：int:N 或 0xADDR
  类方法（_ZN 开头）自动补 this（分配清零对象）。
输出：每个指针参数调用后的内容 hex。
"""
import struct
import re
import sys
import os
import time as htime

from unicorn import *
from unicorn.x86_const import *

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"


def parse_elf(data):
    e_phoff = struct.unpack_from("<I", data, 28)[0]
    e_phentsize = struct.unpack_from("<H", data, 42)[0]
    e_phnum = struct.unpack_from("<H", data, 44)[0]
    phdrs = []
    for i in range(e_phnum):
        off = e_phoff + i * e_phentsize
        p_type, p_offset, p_vaddr, p_paddr, p_filesz, p_memsz, p_flags, p_align = \
            struct.unpack_from("<IIIIIIII", data, off)
        phdrs.append((p_type, p_offset, p_vaddr, p_filesz, p_memsz, p_flags))
    e_shoff = struct.unpack_from("<I", data, 32)[0]
    e_shentsize = struct.unpack_from("<H", data, 46)[0]
    e_shnum = struct.unpack_from("<H", data, 48)[0]
    e_shstrndx = struct.unpack_from("<H", data, 50)[0]
    shdrs = []
    for i in range(e_shnum):
        sh = struct.unpack_from("<IIIIIIIIII", data, e_shoff + i * e_shentsize)
        shdrs.append(sh)
    shstr_off = shdrs[e_shstrndx][4]
    shstr_size = shdrs[e_shstrndx][5]
    shstr = data[shstr_off:shstr_off + shstr_size]
    sections = {}
    for sh in shdrs:
        name = shstr[sh[0]:shstr.find(b"\x00", sh[0])].decode()
        sections[name] = sh
    return phdrs, sections


def load_segments(uc, data, phdrs):
    PAGE = 0x1000
    for p_type, p_offset, p_vaddr, p_filesz, p_memsz, p_flags in phdrs:
        if p_type != 1:
            continue
        start = p_vaddr & ~(PAGE - 1)
        end = (p_vaddr + p_memsz + PAGE - 1) & ~(PAGE - 1)
        uc.mem_map(start, end - start)
        if p_filesz:
            uc.mem_write(p_vaddr, data[p_offset:p_offset + p_filesz])
        if p_memsz > p_filesz:
            uc.mem_write(p_vaddr + p_filesz, b"\x00" * (p_memsz - p_filesz))


def load_symtab(data, sections):
    """返回 {mangled_name: addr}"""
    symtab = sections[".symtab"]
    s_off = symtab[4]
    nsyms = symtab[5] // 16
    str_off = sections[".strtab"][4]
    strtab = data[str_off:]
    out = {}
    for i in range(nsyms):
        st_name, st_value, st_size, st_info, st_other, st_shndx = \
            struct.unpack_from("<IIIBBH", data, s_off + i * 16)
        if st_name and st_value:
            nm = strtab[st_name:strtab.find(b"\x00", st_name)].decode()
            out[nm] = st_value
    return out


def parse_plt(data, sections):
    """返回 {plt_entry_addr: libc_name}（按 rel.plt 顺序）"""
    dynsym = sections.get(".dynsym")
    if dynsym is None or ".rel.plt" not in sections:
        return {}
    sym_off = dynsym[4]
    nsyms = dynsym[5] // 16
    str_off = sections[".dynstr"][4]
    strtab = data[str_off:]
    names = []
    for i in range(nsyms):
        st_name, st_value, st_size, st_info, st_other, st_shndx = \
            struct.unpack_from("<IIIBBH", data, sym_off + i * 16)
        names.append(strtab[st_name:strtab.find(b"\x00", st_name)].decode() if st_name else "")
    rel = sections[".rel.plt"]
    r_off, r_addr, r_size = rel[4], rel[2], rel[5]
    relocs = []
    for i in range(r_size // 8):
        r_offset, r_info = struct.unpack_from("<II", data, r_off + i * 8)
        relocs.append((r_offset, names[r_info >> 8]))
    plt = sections.get(".plt")
    if plt is None:
        return {}
    p_addr = plt[3]   # sh_addr
    out = {}
    for i, (_, name) in enumerate(relocs):
        out[p_addr + 16 + i * 16] = name
    return out


def apply_relocs(uc, data, sections, syms):
    """应用 .rel.dyn 重定位（R_386_32 / R_386_RELATIVE），把内部符号地址
       写入 GOT/数据区（模拟动态链接器的静态重定位）。"""
    # 按节类型（SHT_REL/SHT_RELA）扫描，兼容节名被剥离的二进制
    for name, sh in sections.items():
        sh_type = sh[1]
        if sh_type not in (4, 9):
            continue
        r_off, r_size = sh[4], sh[5]
        link = sh[6]
        for i in range(r_size // 8):
            r_offset, r_info = struct.unpack_from("<II", data, r_off + i * 8)
            rtype = r_info & 0xff
            sym_idx = r_info >> 8
            if rtype in (1, 6):      # R_386_32: S+A；R_386_GLOB_DAT: S
                sym_name = None
                # 用 .dynsym 或链接到的 symtab
                dynsym = sections.get(".dynsym")
                if dynsym is None:
                    continue
                st_name, st_value, st_size, st_info, st_other, st_shndx = \
                    struct.unpack_from("<IIIBBH", data, dynsym[4] + sym_idx * 16)
                str_off = sections[".dynstr"][4]
                strtab = data[str_off:]
                sym_name = strtab[st_name:strtab.find(b"\x00", st_name)].decode() if st_name else ""
                base = syms.get(sym_name, 0)
                if rtype == 1:
                    addend = struct.unpack("<I", uc.mem_read(r_offset, 4))[0]
                    base = (base + addend) & 0xFFFFFFFF
                uc.mem_write(r_offset, struct.pack("<I", base))
            elif rtype == 8:    # R_386_RELATIVE: B + A
                addend = struct.unpack("<I", uc.mem_read(r_offset, 4))[0]
                uc.mem_write(r_offset, struct.pack("<I", (0x08048000 + addend) & 0xFFFFFFFF))


def main():
    os.environ.setdefault("TZ", "UTC")
    htime.tzset()
    args0 = sys.argv[1:]
    auto_this = True
    show_ret = False
    if args0 and args0[0] == "--bin":
        global BIN
        BIN = args0[1]
        args0 = args0[2:]
    if args0 and args0[0] == "--plain":
        auto_this = False
        args0 = args0[1:]
    if args0 and args0[0] == "--ret":
        show_ret = True
        args0 = args0[1:]
    sym = args0[0]
    args = args0[1:]
    data = open(BIN, "rb").read()
    phdrs, sections = parse_elf(data)

    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    load_segments(uc, data, phdrs)
    syms = load_symtab(data, sections)
    apply_relocs(uc, data, sections, syms)
    if sym not in syms:
        raise KeyError(f"symbol not found: {sym}")
    target = syms[sym]
    plt_map = parse_plt(data, sections)

    # 宿主堆与数据区
    uc.mem_map(0x00500000, 0x100000)   # 宿主堆（guest 地址）
    uc.mem_map(0x00600000, 0x10000)    # 数据区
    uc.mem_map(0x00800000, 0x40000)    # 栈
    host_top = 0

    def host_malloc(size):
        nonlocal host_top
        size = (size + 15) & ~15
        if host_top + size > 0x100000:
            raise RuntimeError("host heap exhausted")
        ptr = 0x00500000 + host_top
        host_top += size
        return ptr

    def handle_libc(uc, addr, name):
        esp = uc.reg_read(UC_X86_REG_ESP)
        ret = struct.unpack("<I", uc.mem_read(esp, 4))[0]
        a = [struct.unpack("<I", uc.mem_read(esp + 4 + i * 4, 4))[0] for i in range(4)]
        host_fds = getattr(handle_libc, "host_fds", {})
        next_fd = getattr(handle_libc, "next_fd", 3)
        ERRNO_CELL = 0x0060FF00

        def set_errno(e):
            uc.mem_write(ERRNO_CELL, struct.pack("<i", e))

        def rd_cstr(ptr):
            out = bytearray()
            while True:
                b = uc.mem_read(ptr + len(out), 1)
                if b == b"\x00":
                    return bytes(out)
                out += b

        def wr_cstr(ptr, s):
            if isinstance(s, str):
                s = s.encode("utf-8", "replace")
            uc.mem_write(ptr, s + b"\x00")

        def map_fd(fd):
            if fd in (0, 1, 2):
                return fd
            hfd = host_fds.get(fd)
            if hfd is None:
                raise RuntimeError(f"unknown guest fd {fd}")
            return hfd

        def reg_fd(hfd):
            nonlocal next_fd
            gfd = next_fd
            next_fd += 1
            host_fds[gfd] = hfd
            handle_libc.host_fds = host_fds
            handle_libc.next_fd = next_fd
            return gfd

        def read_va(fmt, va):
            """按 32 位 cdecl 从 va 指针读取一个可变参数，返回 (值, 新va)。"""
            if fmt in ("f", "e", "g"):
                d = struct.unpack("<d", uc.mem_read((va + 7) & ~7, 8))[0]
                return d, va + 8
            if fmt in ("s", "p"):
                return struct.unpack("<I", uc.mem_read(va, 4))[0], va + 4
            if fmt in ("lld", "llu", "llx", "llo"):
                v = struct.unpack("<Q", uc.mem_read((va + 7) & ~7, 8))[0]
                return v, va + 8
            return struct.unpack("<I", uc.mem_read(va, 4))[0], va + 4

        def vformat(fmt_ptr, va):
            fmt = rd_cstr(fmt_ptr)
            out = []
            i = 0
            n = len(fmt)
            while i < n:
                ch = fmt[i]
                if ch != ord("%"):
                    out.append(chr(ch))
                    i += 1
                    continue
                j = i + 1
                while j < n and fmt[j] not in b"diuoxXfFeEgGscp%":
                    j += 1
                if j >= n:
                    out.append(chr(ch))
                    break
                spec = fmt[i:j + 1]
                conv = chr(spec[-1])
                if conv == "%":
                    out.append("%")
                else:
                    if b"ll" in spec or b"L" in spec or b"q" in spec:
                        val, va = read_va("ll" + conv, va)
                    else:
                        val, va = read_va(conv, va)
                    if conv == "s":
                        out.append(rd_cstr(val).decode("latin-1"))
                    elif conv == "p":
                        out.append(f"0x{val:x}")
                    elif conv in ("f", "e", "g"):
                        out.append(spec.decode("latin-1") % val)
                    elif conv == "c":
                        out.append(chr(val & 0xFF))
                    elif conv == "d" or conv == "i":
                        v = val
                        if v & 0x80000000:
                            v = v - 0x100000000
                        spec_s = spec.decode("latin-1")
                        out.append(("%" + spec_s[1:-1].replace("ll", "").replace("l", "") + "d") % v)
                    elif conv == "u":
                        spec_s = spec.decode("latin-1")
                        out.append(("%" + spec_s[1:-1].replace("ll", "").replace("l", "") + "u") % val)
                    else:  # x X o
                        spec_s = spec.decode("latin-1")
                        out.append(("%" + spec_s[1:-1].replace("ll", "").replace("l", "") + conv) % val)
                i = j + 1
            return "".join(out), va

        def do_printf(fmt_ptr, va, cap=None):
            s, _ = vformat(fmt_ptr, va)
            if cap is not None:
                s = s[:cap]
            return s

        if name in ("malloc", "_Znwj", "_Znam", "_Znwm", "_Znaj", "operator new",
                    "_ZnajRKSt9nothrow_t", "_ZnwjRKSt9nothrow_t",
                    "_ZnamRKSt9nothrow_t", "_ZnwmRKSt9nothrow_t"):
            result = host_malloc(a[0])
        elif name in ("free", "_ZdlPv", "_ZdaPv", "_ZdaPv", "operator delete"):
            result = 0
        elif name in ("memcpy", "memmove", "__memcpy_chk"):
            uc.mem_write(a[0], bytes(uc.mem_read(a[1], a[2])))
            result = a[0]
        elif name == "memset":
            uc.mem_write(a[0], bytes([a[1] & 0xFF]) * a[2])
            result = a[0]
        elif name == "memcmp":
            ba = bytes(uc.mem_read(a[0], a[2]))
            bb = bytes(uc.mem_read(a[1], a[2]))
            result = (ba > bb) - (ba < bb)
        elif name == "strlen":
            i = 0
            while uc.mem_read(a[0] + i, 1) != b"\x00":
                i += 1
            result = i
        elif name == "isspace":
            c = a[0] & 0xFF
            result = 1 if c in (0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x20) else 0
        elif name == "tolower":
            c = a[0] & 0xFF
            result = c + 32 if 0x41 <= c <= 0x5A else c
        elif name == "toupper":
            c = a[0] & 0xFF
            result = c - 32 if 0x61 <= c <= 0x7A else c
        elif name in ("strcmp",):
            ba = rd_cstr(a[0])
            bb = rd_cstr(a[1])
            result = (ba > bb) - (ba < bb)
        elif name == "strncmp":
            ba = bytes(uc.mem_read(a[0], a[2]))
            bb = bytes(uc.mem_read(a[1], a[2]))
            result = (ba > bb) - (ba < bb)
        elif name in ("strcasecmp", "strncasecmp"):
            if name == "strcasecmp":
                ba = rd_cstr(a[0])
                bb = rd_cstr(a[1])
            else:
                ba = bytes(uc.mem_read(a[0], a[2]))
                bb = bytes(uc.mem_read(a[1], a[2]))

            def low(x):
                return x + 32 if 0x41 <= x <= 0x5A else x

            i = 0
            n = len(ba) if name == "strncasecmp" else min(len(ba), len(bb)) + 1
            result = 0
            while i < n:
                x = low(ba[i] if i < len(ba) else 0)
                y = low(bb[i] if i < len(bb) else 0)
                if x != y:
                    result = x - y
                    break
                if x == 0:
                    break
                i += 1
        elif name == "sscanf":
            src = rd_cstr(a[0]).decode("latin-1")
            fmt = rd_cstr(a[1]).decode("latin-1")
            out_ptrs = []
            i = 2
            while i < len(a) and a[i] != 0:
                out_ptrs.append(a[i])
                i += 1
            si = 0
            fi = 0
            assigned = 0
            n = len(src)

            def skip_ws(idx):
                while idx < n and src[idx] in " \t\r\n\v\f":
                    idx += 1
                return idx

            while fi < len(fmt) and assigned < len(out_ptrs):
                ch = fmt[fi]
                if ch == " ":
                    si = skip_ws(si)
                    fi += 1
                    continue
                if ch != "%":
                    if si < n and src[si] == ch:
                        si += 1
                        fi += 1
                        continue
                    break
                j = fi + 1
                width = ""
                while j < len(fmt) and fmt[j].isdigit():
                    width += fmt[j]
                    j += 1
                conv = fmt[j] if j < len(fmt) else ""
                w = int(width) if width else 0
                si = skip_ws(si)
                start = si
                if si < n and src[si] in "+-":
                    si += 1
                digits = 0
                while si < n and src[si].isdigit() and (w == 0 or digits < w):
                    si += 1
                    digits += 1
                if digits == 0:
                    break
                val = int(src[start:si], 10)
                uc.mem_write(out_ptrs[assigned], struct.pack("<i", val))
                assigned += 1
                fi = j + 1
            result = assigned
        elif name == "mktime":
            vals = struct.unpack("<9i", uc.mem_read(a[0], 36))
            t = htime.mktime((vals[5] + 1900, vals[4] + 1, vals[3], vals[2],
                              vals[1], vals[0], vals[6], vals[7], vals[8]))
            lt = htime.localtime(t)
            uc.mem_write(a[0] + 24, struct.pack("<i", lt.tm_wday))
            uc.mem_write(a[0] + 28, struct.pack("<i", lt.tm_yday))
            result = int(t) & 0xFFFFFFFF
        elif name in ("localtime_r", "gmtime_r"):
            sec = struct.unpack("<i", uc.mem_read(a[0], 4))[0]
            if name == "localtime_r":
                lt = htime.localtime(sec)
            else:
                lt = htime.gmtime(sec)
            vals = [lt.tm_sec, lt.tm_min, lt.tm_hour, lt.tm_mday, lt.tm_mon,
                    lt.tm_year - 1900, lt.tm_wday, lt.tm_yday, lt.tm_isdst]
            uc.mem_write(a[1], struct.pack("<9i", *vals))
            result = a[1]
        elif name == "strcpy":
            s = rd_cstr(a[1])
            wr_cstr(a[0], s)
            result = a[0]
        elif name == "strncpy":
            s = bytes(uc.mem_read(a[1], a[2]))
            uc.mem_write(a[0], s + b"\x00" * max(0, a[2] - len(s)))
            result = a[0]
        elif name == "strcat":
            cur = a[0]
            while uc.mem_read(cur, 1) != b"\x00":
                cur += 1
            s = rd_cstr(a[1])
            uc.mem_write(cur, s + b"\x00")
            result = a[0]
        elif name in ("strchr", "strrchr"):
            s = rd_cstr(a[0])
            b = a[1] & 0xFF
            idx = s.rfind(bytes([b])) if name == "strrchr" else s.find(bytes([b]))
            result = a[0] + idx if idx >= 0 else 0
        elif name == "strstr":
            s = rd_cstr(a[0])
            sub = rd_cstr(a[1])
            idx = s.find(sub)
            result = a[0] + idx if idx >= 0 else 0
        elif name == "memchr":
            s = bytes(uc.mem_read(a[0], a[2]))
            idx = s.find(bytes([a[1] & 0xFF]))
            result = a[0] + idx if idx >= 0 else 0
        elif name in ("atoi", "atol"):
            s = rd_cstr(a[0]).decode("latin-1").strip()
            result = int(s, 0) if s else 0
        elif name in ("strtol", "strtoul", "strtoll", "strtoull",
                      "__strtol_internal", "__strtoul_internal",
                      "__strtoll_internal", "__strtoull_internal"):
            is64 = name in ("strtoll", "strtoull", "__strtoll_internal",
                            "__strtoull_internal")
            unsigned = name in ("strtoul", "strtoull", "__strtoul_internal",
                                "__strtoull_internal")
            s = rd_cstr(a[0]).decode("latin-1")
            base = a[2]
            i = 0
            n = len(s)
            while i < n and s[i] in " \t\r\n\v\f":
                i += 1
            sign = 1
            if i < n and s[i] in "+-":
                if s[i] == "-":
                    sign = -1
                i += 1
            b = base if base else 0
            digits = 0
            val = 0
            consumed = i
            if b == 0 or b == 16:
                if i + 1 < n and s[i] == "0" and s[i + 1] in "xX":
                    i += 2
                    b = 16
            elif b == 8 and i < n and s[i] == "0":
                b = 8
            if b == 0:
                if i < n and s[i] == "0":
                    b = 8
                else:
                    b = 10
            while i < n:
                ch = s[i]
                if "0" <= ch <= "9":
                    d = ord(ch) - ord("0")
                elif "a" <= ch.lower() <= "f":
                    d = ord(ch.lower()) - ord("a") + 10
                else:
                    break
                if d < b:
                    val = val * b + d
                    digits += 1
                    i += 1
                else:
                    break
            if digits == 0:
                end = a[0] + consumed
                val = 0
            else:
                end = a[0] + i
                val = val * sign
            if a[1]:
                uc.mem_write(a[1], struct.pack("<I", end))
            # glibc 溢出饱和（strtol/strtoll 返回 LONG_MAX 等）
            if is64:
                if unsigned:
                    if val < 0:
                        val &= 0xFFFFFFFFFFFFFFFF
                    if val > 0xFFFFFFFFFFFFFFFF:
                        val = 0xFFFFFFFFFFFFFFFF
                else:
                    if val > 0x7FFFFFFFFFFFFFFF:
                        val = 0x7FFFFFFFFFFFFFFF
                    if val < -0x8000000000000000:
                        val = -0x8000000000000000
                uc.reg_write(UC_X86_REG_EDX, (val >> 32) & 0xFFFFFFFF)
                result = val & 0xFFFFFFFF
            else:
                if unsigned:
                    if val < 0:
                        val &= 0xFFFFFFFF
                    if val > 0xFFFFFFFF:
                        val = 0xFFFFFFFF
                else:
                    if val > 0x7FFFFFFF:
                        val = 0x7FFFFFFF
                    if val < -0x80000000:
                        val = -0x80000000
                result = val & 0xFFFFFFFF
        elif name in ("strtod", "__strtod_internal"):
            s = rd_cstr(a[0]).decode("latin-1")
            i = 0
            n = len(s)
            while i < n and s[i] in " \t\r\n\v\f":
                i += 1
            m = re.match(r"^[+-]?(\d+\.?\d*|\.\d+)([eE][+-]?\d+)?", s[i:])
            if not m:
                end = a[0] + i
                v = 0.0
            else:
                tok = m.group(0)
                try:
                    v = float(tok)
                except ValueError:
                    v = 0.0
                end = a[0] + i + len(tok)
            if a[1]:
                uc.mem_write(a[1], struct.pack("<I", end))
            # x87 返回无法直接写 ST0（unicorn reg API 不作用于 FPU 栈），
            # 由 on_code 里的 fstp 指令拦截把值写入目标内存。
            FP_PENDING[0] = v
            result = 0
        elif name == "strtok_r":
            if a[0]:
                cur = a[0]
            else:
                cur = struct.unpack("<I", uc.mem_read(a[2], 4))[0]
            delim = rd_cstr(a[1])
            while cur:
                b = bytes(uc.mem_read(cur, 1))
                if b == b"\x00":
                    cur = 0
                    break
                if b in delim:
                    cur += 1
                else:
                    break
            if cur == 0:
                uc.mem_write(a[2], struct.pack("<I", 0))
                result = 0
            else:
                tok = cur
                while True:
                    b = bytes(uc.mem_read(cur, 1))
                    if b == b"\x00":
                        uc.mem_write(a[2], struct.pack("<I", 0))
                        break
                    if b in delim:
                        uc.mem_write(cur, b"\x00")
                        uc.mem_write(a[2], struct.pack("<I", cur + 1))
                        break
                    cur += 1
                result = tok
        elif name == "inet_addr":
            s = rd_cstr(a[0]).decode("latin-1")
            try:
                parts = [int(x) for x in s.split(".")]
                v = (parts[0] & 0xFF) | ((parts[1] & 0xFF) << 8) | \
                    ((parts[2] & 0xFF) << 16) | ((parts[3] & 0xFF) << 24)
                result = v & 0xFFFFFFFF
            except Exception:
                result = 0xFFFFFFFF
        elif name == "inet_ntoa":
            v = struct.unpack("<I", uc.mem_read(a[0], 4))[0]
            s = f"{(v & 0xFF)}.{(v >> 8) & 0xFF}.{(v >> 16) & 0xFF}.{(v >> 24) & 0xFF}"
            p = host_malloc(len(s) + 1)
            wr_cstr(p, s)
            result = p
        elif name in ("htonl", "ntohl"):
            result = ((a[0] & 0xFF) << 24) | ((a[0] & 0xFF00) << 8) | \
                     ((a[0] >> 8) & 0xFF00) | ((a[0] >> 24) & 0xFF)
        elif name in ("htons", "ntohs"):
            result = ((a[0] & 0xFF) << 8) | ((a[0] >> 8) & 0xFF)
        elif name in ("inet_aton",):
            s = rd_cstr(a[0]).decode("latin-1")
            try:
                parts = [int(x) for x in s.split(".")]
                v = (parts[0] & 0xFF) | ((parts[1] & 0xFF) << 8) | \
                    ((parts[2] & 0xFF) << 16) | ((parts[3] & 0xFF) << 24)
                if a[1]:
                    uc.mem_write(a[1], struct.pack("<I", v))
                result = 1
            except Exception:
                result = 0
        elif name == "inet_pton":
            family, src, dst = a[0], a[1], a[2]
            s = rd_cstr(src).decode("latin-1")
            if family == 2:  # AF_INET
                parts = s.split(".")
                try:
                    vals = [int(p) for p in parts]
                    if len(vals) == 4 and all(0 <= v <= 255 for v in vals):
                        uc.mem_write(dst, bytes(vals))
                        result = 1
                    else:
                        result = 0
                except ValueError:
                    result = 0
            elif family == 10:  # AF_INET6, minimal deterministic parser
                try:
                    groups = []
                    for g in s.split(":"):
                        if g == "":
                            groups.append(0)
                        elif len(g) <= 4:
                            groups.append(int(g, 16))
                        else:
                            raise ValueError
                    if len(groups) == 8:
                        uc.mem_write(dst, b"".join(struct.pack(">H", g) for g in groups))
                        result = 1
                    else:
                        result = 0
                except ValueError:
                    result = 0
            else:
                set_errno(97)
                result = -1
        elif name == "inet_ntop":
            family, src, dst, size = a[0], a[1], a[2], a[3]
            if family == 2:
                b = bytes(uc.mem_read(src, 4))
                s = "%d.%d.%d.%d" % tuple(b)
            elif family == 10:
                b = bytes(uc.mem_read(src, 16))
                groups = [int.from_bytes(b[i:i + 2], "big") for i in range(0, 16, 2)]
                s = ":".join("%x" % g for g in groups)
            else:
                set_errno(97)
                result = 0
                s = None
            if s is not None:
                if len(s) + 1 > size:
                    set_errno(28)
                    result = 0
                else:
                    wr_cstr(dst, s)
                    result = dst
        elif name == "getnameinfo":
            sa = a[0]
            salen = a[1]
            host, hostlen, serv, servlen = a[2], a[3], a[4], a[5]
            family = struct.unpack("<H", uc.mem_read(sa, 2))[0]
            if family == 2 and salen >= 8:
                b = bytes(uc.mem_read(sa + 4, 4))
                port = struct.unpack(">H", uc.mem_read(sa + 2, 2))[0]
                ip = "%d.%d.%d.%d" % tuple(b)
            elif family == 10 and salen >= 24:
                b = bytes(uc.mem_read(sa + 8, 16))
                port = struct.unpack(">H", uc.mem_read(sa + 2, 2))[0]
                groups = [int.from_bytes(b[i:i + 2], "big") for i in range(0, 16, 2)]
                ip = ":".join("%x" % g for g in groups)
            else:
                result = -1
                ip = None
            if ip is not None:
                if host and hostlen:
                    wr_cstr(host, ip[:max(0, hostlen - 1)])
                if serv and servlen:
                    wr_cstr(serv, str(port)[:max(0, servlen - 1)])
                result = 0
        elif name == "setsockopt":
            result = 0
        elif name in ("printf", "__printf_chk"):
            s = do_printf(a[1] if name == "__printf_chk" else a[0],
                          a[2] if name == "__printf_chk" else a[1])
            sys.stdout.write(s)
            sys.stdout.flush()
            result = len(s)
        elif name in ("puts",):
            s = rd_cstr(a[0]).decode("latin-1")
            sys.stdout.write(s + "\n")
            sys.stdout.flush()
            result = len(s) + 1
        elif name in ("fprintf",):
            s = do_printf(a[1], a[2])
            sys.stderr.write(s)
            sys.stderr.flush()
            result = len(s)
        elif name in ("sprintf", "snprintf", "__sprintf_chk", "__snprintf_chk"):
            if name == "sprintf":
                off = 1
            elif name == "__sprintf_chk":
                off = 3
            elif name == "snprintf":
                off = 2
            else:  # __snprintf_chk
                off = 4
            if name in ("sprintf", "__sprintf_chk"):
                s = do_printf(a[off], a[off + 1])
                wr_cstr(a[0], s)
                result = len(s)
            else:
                cap = a[1]
                s = do_printf(a[off], a[off + 1], cap)
                if len(s) >= cap and cap > 0:
                    uc.mem_write(a[0], s[:cap - 1].encode("latin-1") + b"\x00")
                else:
                    uc.mem_write(a[0], s.encode("latin-1") + b"\x00")
                result = len(s)
        elif name == "vsnprintf":
            # vsnprintf(char* str, size_t size, const char* format, va_list ap)
            s = do_printf(a[2], a[3], a[1])
            if a[1] > 0:
                if len(s) >= a[1]:
                    uc.mem_write(a[0], s[:a[1] - 1].encode("latin-1") + b"\x00")
                else:
                    uc.mem_write(a[0], s.encode("latin-1") + b"\x00")
            result = len(s)
        elif name in ("open", "open64", "creat"):
            path = rd_cstr(a[0]).decode("latin-1")
            flags = a[1]
            mode = (a[2] if len(a) > 2 and (flags & 0o100) else 0o644)
            try:
                hfd = os.open(path, flags & ~0o100000, mode)
                result = reg_fd(hfd)
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "close":
            try:
                os.close(map_fd(a[0]))
                result = 0
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "read":
            try:
                data = os.read(map_fd(a[0]), a[2])
                uc.mem_write(a[1], data)
                result = len(data)
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "write":
            try:
                data = bytes(uc.mem_read(a[1], a[2]))
                result = os.write(map_fd(a[0]), data)
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "lseek":
            try:
                result = os.lseek(map_fd(a[0]), a[1], a[2])
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "__errno_location":
            result = ERRNO_CELL
        elif name in ("fstat", "stat", "lstat"):
            if name == "fstat":
                st = os.fstat(map_fd(a[0]))
            else:
                st = os.stat(rd_cstr(a[0]).decode("latin-1"))
            # struct stat (i386): 填充常用字段（st_size @+0x28, st_mode @+0x18, st_mtime @+0x2c）
            uc.mem_write(a[1], struct.pack("<IIIIIIII", st.st_dev, st.st_ino, st.st_mode,
                                           st.st_nlink, st.st_uid, st.st_gid, st.st_rdev, 0))
            uc.mem_write(a[1] + 0x20, struct.pack("<IIIII", st.st_size, st.st_blksize,
                                                   st.st_blocks, st.st_atime, st.st_mtime))
            result = 0
        elif name in ("__xstat64", "__fxstat64", "__lxstat64"):
            if name == "__fxstat64":
                st = os.fstat(map_fd(a[1]))
            else:
                st = os.stat(rd_cstr(a[1]).decode("latin-1"))
            # i386 glibc stat64 布局：dev@0(8) pad@8(4) st_ino32@12 mode@16 nlink@20
            # uid@24 gid@28 rdev@32(8) pad@40 size@44 blksize@52 blocks@56(8)
            # atime@64 mtime@72 ctime@80 st_ino64@88
            buf = struct.pack("<QIIIIIIQIQ", int(st.st_dev), 0, int(st.st_ino) & 0xFFFFFFFF,
                              int(st.st_mode), int(st.st_nlink), int(st.st_uid), int(st.st_gid),
                              int(st.st_rdev), 0, int(st.st_size))
            buf += struct.pack("<IIQIIIIIIQ", int(st.st_blksize), 0, int(st.st_blocks),
                               int(st.st_atime), 0, int(st.st_mtime), 0, int(st.st_ctime), 0,
                               int(st.st_ino))
            uc.mem_write(a[2], buf)
            result = 0
        elif name == "access":
            if os.access(rd_cstr(a[0]).decode("latin-1"), a[1]):
                result = 0
            else:
                set_errno(2)
                result = -1
        elif name in ("unlink",):
            try:
                os.unlink(rd_cstr(a[0]).decode("latin-1"))
                result = 0
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name in ("rename",):
            try:
                os.rename(rd_cstr(a[0]).decode("latin-1"), rd_cstr(a[1]).decode("latin-1"))
                result = 0
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "ftruncate":
            try:
                os.ftruncate(map_fd(a[0]), a[1])
                result = 0
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "fsync":
            try:
                os.fsync(map_fd(a[0]))
                result = 0
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "dup":
            result = reg_fd(os.dup(map_fd(a[0])))
        elif name == "dup2":
            try:
                os.dup2(map_fd(a[0]), a[1])
                result = a[1]
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "pipe":
            r, w = os.pipe()
            uc.mem_write(a[0], struct.pack("<II", reg_fd(r), reg_fd(w)))
            result = 0
        elif name == "fcntl":
            try:
                if a[1] == 3:  # F_GETFL
                    result = os.fcntl(map_fd(a[0]), a[1])
                else:
                    result = os.fcntl(map_fd(a[0]), a[1], a[2])
            except OSError as e:
                set_errno(e.errno)
                result = -1
        elif name == "gettimeofday":
            if os.environ.get("UNI_FAKE_TIME"):
                sec, usec = [int(x) for x in os.environ["UNI_FAKE_TIME"].split(".")]
            else:
                now = htime.time()
                sec = int(now)
                usec = int((now - sec) * 1e6)
            if a[0]:
                uc.mem_write(a[0], struct.pack("<II", sec, usec))
            result = 0
        elif name in ("clock_gettime", "clock_gettime64"):
            if os.environ.get("UNI_FAKE_TIME"):
                sec, usec = [int(x) for x in os.environ["UNI_FAKE_TIME"].split(".")]
                nsec = usec * 1000
            else:
                now = htime.time()
                sec = int(now)
                nsec = int((now - sec) * 1e9)
            uc.mem_write(a[1], struct.pack("<II", sec, nsec))
            result = 0
        elif name == "time":
            t = int(htime.time())
            if a[0]:
                uc.mem_write(a[0], struct.pack("<I", t))
            result = t
        elif name == "nanosleep":
            s, ns = struct.unpack("<II", uc.mem_read(a[0], 8))
            htime.sleep(s + ns / 1e9)
            result = 0
        elif name in ("usleep",):
            htime.sleep(a[0] / 1e6)
            result = 0
        elif name == "sleep":
            htime.sleep(a[0])
            result = 0
        elif name in ("__cxa_guard_acquire", "__cxa_guard_release",
                      "pthread_mutex_lock", "pthread_mutex_unlock",
                      "pthread_once", "pthread_self", "pthread_equal",
                      "pthread_cond_wait", "pthread_cond_signal",
                      "pthread_cond_broadcast", "pthread_mutexattr_init",
                      "pthread_mutexattr_settype", "pthread_attr_init",
                      "pthread_attr_setstacksize", "pthread_create",
                      "pthread_join", "pthread_detach", "pthread_exit",
                      "pthread_key_create", "pthread_setspecific",
                      "pthread_getspecific"):
            result = 0
        elif name in ("getpid", "getppid", "getuid", "geteuid", "getgid"):
            if name == "getpid" and os.environ.get("UNI_FAKE_PID"):
                result = int(os.environ["UNI_FAKE_PID"])
            else:
                result = getattr(os, name)()
        elif name == "freeaddrinfo":
            result = 0
        elif name == "getaddrinfo":
            host = rd_cstr(a[0]).decode("latin-1")
            hints = a[2]
            res = a[3]
            if hints:
                family = struct.unpack("<i", uc.mem_read(hints + 4, 4))[0]
            else:
                family = 0
            if host != "test.local":
                result = -2  # EAI_NONAME
                uc.mem_write(res, struct.pack("<I", 0))
            else:
                ai = host_malloc(32)
                sa = host_malloc(16)
                uc.mem_write(sa, struct.pack("<HHI", 2, 0x1234, 0x7F000001))
                uc.mem_write(ai + 0, struct.pack("<iiii", 0, family, 2, 0))
                uc.mem_write(ai + 16, struct.pack("<IIII", 16, sa, 0, 0))
                uc.mem_write(res, struct.pack("<I", ai))
                result = 0
        elif name == "gethostbyname":
            host = rd_cstr(a[0]).decode("latin-1")
            if host != "test.local":
                result = 0
            else:
                he = host_malloc(32)
                al = host_malloc(12)
                addr = host_malloc(4)
                nm = host_malloc(16)
                uc.mem_write(nm, b"test.local\x00")
                uc.mem_write(addr, struct.pack("<I", 0x7F000001))
                uc.mem_write(al, struct.pack("<III", addr, 0, 0))
                uc.mem_write(he, struct.pack("<IIIIII", nm, al, 2, 4, 0, 0))
                result = he
        elif name in ("__divdi3", "__moddi3", "__udivdi3", "__umoddi3"):
            def to_s64(v):
                return v - 0x10000000000000000 if v & 0x8000000000000000 else v
            x = (a[1] << 32) | a[0]
            y = (a[3] << 32) | a[2]
            if name in ("__udivdi3", "__umoddi3"):
                q, r = divmod(x, y) if y else (0, 0)
            else:
                xs, ys = to_s64(x), to_s64(y)
                if ys == 0:
                    q = r = 0
                else:
                    q, r = divmod(abs(xs), abs(ys))
                    if (xs < 0) != (ys < 0):
                        q = -q
                    if xs < 0:
                        r = -r
                q &= 0xFFFFFFFFFFFFFFFF
                r &= 0xFFFFFFFFFFFFFFFF
            v = q if name in ("__divdi3", "__udivdi3") else r
            # GCC 4.1 i386：64 位返回走 edx:eax
            uc.reg_write(UC_X86_REG_EDX, (v >> 32) & 0xFFFFFFFF)
            result = v & 0xFFFFFFFF
        elif name == "strptime":
            s = rd_cstr(a[0]).decode("latin-1")
            fmt = rd_cstr(a[1]).decode("latin-1")
            tm = a[2]
            si = 0
            fi = 0
            n = len(s)
            m = len(fmt)
            vals = {"Y": None, "m": None, "d": None, "H": None, "M": None, "S": None}

            def skip_ws(idx):
                while idx < n and s[idx] in " \t\r\n\v\f":
                    idx += 1
                return idx

            def read_num(idx, width, lo, hi):
                j = 0
                while idx + j < n and s[idx + j].isdigit() and (width == 0 or j < width):
                    j += 1
                if j == 0:
                    return None, 0
                v = int(s[idx:idx + j])
                return (v, j) if lo <= v <= hi else (None, 0)

            ok = True
            while fi < m:
                ch = fmt[fi]
                if ch == "%":
                    if fi + 1 >= m:
                        ok = False
                        break
                    conv = fmt[fi + 1]
                    fi += 2
                    if conv == "%":
                        if si < n and s[si] == "%":
                            si += 1
                            continue
                        ok = False
                        break
                    width, lo, hi = {"Y": (4, 0, 9999), "m": (2, 1, 12),
                                     "d": (2, 1, 31), "H": (2, 0, 23),
                                     "M": (2, 0, 59), "S": (2, 0, 60)}.get(conv, (0, 0, 0))
                    if conv not in vals:
                        ok = False
                        break
                    v, consumed = read_num(si, width, lo, hi)
                    if v is None:
                        ok = False
                        break
                    vals[conv] = v
                    si += consumed
                elif ch in " \t":
                    si = skip_ws(si)
                    fi += 1
                else:
                    if si < n and s[si] == ch:
                        si += 1
                        fi += 1
                    else:
                        ok = False
                        break
            if not ok:
                result = 0
            else:
                tm_vals = [vals["S"] if vals["S"] is not None else 0,
                           vals["M"] if vals["M"] is not None else 0,
                           vals["H"] if vals["H"] is not None else 0,
                           vals["d"] if vals["d"] is not None else 0,
                           (vals["m"] - 1) if vals["m"] is not None else 0,
                           (vals["Y"] - 1900) if vals["Y"] is not None else 0,
                           0, 0, 0]
                uc.mem_write(tm, struct.pack("<9i", *tm_vals))
                result = a[0] + si
        elif name in ("exit", "_exit", "abort"):
            print(f"[uni_call] {name}() called, stopping")
            uc.emu_stop()
            result = 0
        else:
            raise RuntimeError(f"unhandled libc call: {name} @ {addr:#x}")
        if os.environ.get("UNI_DEBUG"):
            print(f"[libc] {name} args={[hex(x) for x in a]} -> {result:#x}", flush=True)
        uc.reg_write(UC_X86_REG_EAX, result & 0xFFFFFFFF)
        uc.reg_write(UC_X86_REG_ESP, esp + 4)
        uc.reg_write(UC_X86_REG_EIP, ret)

    FP_PENDING = {}

    def on_code(uc, address, size, user_data):
        if FP_PENDING:
            code = bytes(uc.mem_read(address, min(size, 7)))
            op = None
            if len(code) >= 2 and code[0] == 0xDD and ((code[1] >> 3) & 7) == 3:
                op = "d"          # fstpl
            elif len(code) >= 2 and code[0] == 0xD9 and ((code[1] >> 3) & 7) == 3:
                op = "f"          # fstps
            if op is not None:
                regmap = {"EAX": UC_X86_REG_EAX, "ECX": UC_X86_REG_ECX,
                          "EDX": UC_X86_REG_EDX, "EBX": UC_X86_REG_EBX,
                          "ESP": UC_X86_REG_ESP, "EBP": UC_X86_REG_EBP,
                          "ESI": UC_X86_REG_ESI, "EDI": UC_X86_REG_EDI}
                modrm = code[1]
                mod = modrm >> 6
                rm = modrm & 7
                length = 2
                ea = 0
                if mod == 3:
                    FP_PENDING.clear()
                    return
                disp = 0
                has_sib = rm == 4
                if has_sib:
                    sib = code[2]
                    base = sib & 7
                    index = (sib >> 3) & 7
                    scale = 1 << (sib >> 6)
                    length = 3
                    if mod == 0 and base == 5:
                        disp = struct.unpack("<i", code[3:7])[0]
                        length = 7
                        ea = disp
                    else:
                        regs = ["EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"]
                        ea = uc.reg_read(regmap[regs[base]])
                        if index != 4:
                            ea += uc.reg_read(regmap[regs[index]]) * scale
                        if mod == 1:
                            disp = struct.unpack("<b", code[3:4])[0]
                            length = 4
                            ea += disp
                        elif mod == 2:
                            disp = struct.unpack("<i", code[4:8])[0]
                            length = 7
                            ea += disp
                else:
                    regs = ["EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI"]
                    if mod == 0 and rm == 5:
                        disp = struct.unpack("<i", code[2:6])[0]
                        length = 6
                        ea = disp
                    else:
                        ea = uc.reg_read(regmap[regs[rm]])
                        if mod == 1:
                            disp = struct.unpack("<b", code[2:3])[0]
                            length = 3
                            ea += disp
                        elif mod == 2:
                            disp = struct.unpack("<i", code[3:7])[0]
                            length = 6
                            ea += disp
                v = FP_PENDING.pop(0)
                if op == "d":
                    uc.mem_write(ea, struct.pack("<d", v))
                else:
                    uc.mem_write(ea, struct.pack("<f", v))
                uc.reg_write(UC_X86_REG_EIP, address + length)
                return
        if address in plt_map:
            handle_libc(uc, address, plt_map[address])

    uc.hook_add(UC_HOOK_CODE, on_code)

    done = {"hit": False}

    def on_unmapped(uc, access, address, size, value, user_data):
        if address == 0xCCCCCCCC:
            done["hit"] = True
            uc.emu_stop()
            return True
        return False

    uc.hook_add(UC_HOOK_MEM_FETCH_UNMAPPED, on_unmapped)

    # ---- int 0x80 系统调用模拟（用于运行我们自己的静态 32 位测试程序）----
    BRK_BASE = 0x00700000
    BRK_TOP = 0x00780000
    uc.mem_map(BRK_BASE, BRK_TOP - BRK_BASE)
    brk_state = {"cur": BRK_BASE}
    mmap_state = {"top": BRK_TOP}

    def on_intr(uc, intno, user_data):
        if intno != 0x80:
            return
        nr = uc.reg_read(UC_X86_REG_EAX)
        ebx = uc.reg_read(UC_X86_REG_EBX)
        ecx = uc.reg_read(UC_X86_REG_ECX)
        edx = uc.reg_read(UC_X86_REG_EDX)
        esi = uc.reg_read(UC_X86_REG_ESI)
        edi = uc.reg_read(UC_X86_REG_EDI)
        ret = 0
        if nr in (1, 2):      # exit / fork
            print(f"[syscall] exit({ebx})", flush=True)
            uc.emu_stop()
            return
        elif nr == 3:         # read
            try:
                data = os.read(ebx if ebx < 3 else handle_libc.host_fds.get(ebx, ebx), edx)
                uc.mem_write(ecx, data)
                ret = len(data)
            except OSError as e:
                ret = -e.errno
        elif nr == 4:         # write
            try:
                data = bytes(uc.mem_read(ecx, edx))
                fd = ebx if ebx < 3 else handle_libc.host_fds.get(ebx, ebx)
                ret = os.write(fd, data)
            except OSError as e:
                ret = -e.errno
        elif nr == 5:         # open
            path = bytes(uc.mem_read(ebx, 256)).split(b"\x00")[0].decode("latin-1")
            flags = ecx
            mode = edx if (flags & 0o100) else 0o644
            try:
                hfd = os.open(path, flags & ~0o100000, mode)
                gfd = handle_libc.next_fd
                handle_libc.next_fd += 1
                handle_libc.host_fds[gfd] = hfd
                ret = gfd
            except OSError as e:
                ret = -e.errno
        elif nr == 6:         # close
            try:
                os.close(ebx if ebx < 3 else handle_libc.host_fds.get(ebx, ebx))
                ret = 0
            except OSError:
                ret = -9
        elif nr == 45:        # brk
            if ebx == 0:
                ret = brk_state["cur"]
            elif BRK_BASE <= ebx <= BRK_TOP:
                brk_state["cur"] = ebx
                ret = ebx
            else:
                ret = brk_state["cur"]
        elif nr in (19,):      # lseek
            try:
                fd = ebx if ebx < 3 else handle_libc.host_fds.get(ebx, ebx)
                ret = os.lseek(fd, ecx, edx)
            except OSError as e:
                uc.mem_write(0x0060FF00, struct.pack("<i", e.errno))
                ret = -1
        elif nr == 78:        # gettimeofday
            now = htime.time()
            if ecx:
                uc.mem_write(ecx, struct.pack("<II", int(now), int((now - int(now)) * 1e6)))
            ret = 0
        elif nr == 90:        # mmap
            # ebx=addr ecx=len edx=prot esi=flags edi=fd ebp=offset
            size = (ecx + 0xFFF) & ~0xFFF
            addr = mmap_state["top"]
            if addr < brk_state["cur"] + 0x1000:
                addr = (brk_state["cur"] + 0xFFF) & ~0xFFF
            try:
                uc.mem_map(addr, size)
            except UcError:
                pass
            mmap_state["top"] = addr + size
            ret = addr
        elif nr == 91:        # munmap
            ret = 0
        elif nr in (33,):     # access
            path = bytes(uc.mem_read(ebx, 256)).split(b"\x00")[0].decode("latin-1")
            ret = 0 if os.access(path, ecx) else -1
        elif nr in (76,):     # getrlimit
            import resource
            lim = resource.getrlimit(ebx)
            if ecx:
                uc.mem_write(ecx, struct.pack("<qq", lim[0], lim[1]))
            ret = 0
        elif nr in (116,):    # sysinfo
            if ebx:
                uc.mem_write(ebx, b"\x00" * 64)
            ret = 0
        elif nr == 122:       # uname
            if ebx:
                base = ebx
                for i, s in enumerate(["Linux", "host", "5.15", "1", "i686", "GNU/Linux"]):
                    b = s.encode() + b"\x00"
                    uc.mem_write(base + i * 65, b)
            ret = 0
        elif nr in (125, 174, 175, 192, 224, 240, 252, 295, 296, 297, 298):
            # mprotect / rt_sigaction / rt_sigprocmask / getuid / gettid /
            # futex(0) / exit_group / openat... 简单处理
            if nr in (252, 1):
                print(f"[syscall] exit_group({ebx})", flush=True)
                uc.emu_stop()
                return
            ret = 0
        elif nr in (195, 196, 197, 198):   # stat64/lstat64/fstat64/stat
            path = bytes(uc.mem_read(ebx, 256)).split(b"\x00")[0].decode("latin-1")
            try:
                st = os.stat(path)
            except OSError:
                ret = -1
            else:
                buf = struct.pack("<QIIIIIIQIQ", int(st.st_dev), 0,
                                  int(st.st_ino) & 0xFFFFFFFF, int(st.st_mode),
                                  int(st.st_nlink), int(st.st_uid), int(st.st_gid),
                                  int(st.st_rdev), 0, int(st.st_size))
                buf += struct.pack("<IIQIIIIIIQ", int(st.st_blksize), 0,
                                   int(st.st_blocks), int(st.st_atime), 0,
                                   int(st.st_mtime), 0, int(st.st_ctime), 0,
                                   int(st.st_ino))
                uc.mem_write(ecx, buf)
                ret = 0
        elif nr in (20, 24, 47, 49, 50, 64, 224):
            ret = 0
        elif nr == 162:       # nanosleep
            htime.sleep(0.001)
            ret = 0
        else:
            if os.environ.get("UNI_DEBUG"):
                print(f"[syscall] nr={nr} ebx={ebx:#x} ecx={ecx:#x} edx={edx:#x}", flush=True)
            ret = 0
        uc.reg_write(UC_X86_REG_EAX, ret & 0xFFFFFFFF)

    uc.hook_add(UC_HOOK_INTR, on_intr)

    def on_mem_unmapped(uc, access, address, size, value, user_data):
        print(f"MEM_UNMAPPED access={access} addr={address:#x} size={size}", flush=True)
        return False

    uc.hook_add(UC_HOOK_MEM_WRITE_UNMAPPED, on_mem_unmapped)
    uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED, on_mem_unmapped)

    # 参数
    # w:ADDR:HEX 直接写 guest 内存；r:ADDR:LEN 调用后读回并打印
    writes = []
    reads = []
    filtered = []
    for a in args:
        if a.startswith("w:"):
            writes.append(a)
        elif a.startswith("r:"):
            reads.append(a)
        else:
            filtered.append(a)
    args = filtered
    for a in writes:
        parts = a.split(":", 2)
        if len(parts) != 3:
            raise ValueError("bad write arg: %s" % a)
        uc.mem_write(int(parts[1], 16), bytes.fromhex(parts[2]))
    datas = []
    call_args = []
    for a in args:
        if a.startswith("int:"):
            call_args.append(int(a[4:], 0) & 0xFFFFFFFF)
        elif a.startswith("0x"):
            call_args.append(int(a, 16) & 0xFFFFFFFF)
        else:
            datas.append(bytes.fromhex(a))
            call_args.append(None)

    DATA_BASE = 0x00600000
    data_ptrs = []
    off = 0
    for d in datas:
        ptr = DATA_BASE + off
        data_ptrs.append(ptr)
        uc.mem_write(ptr, d + b"\x00" * 16)
        off += len(d) + 16
    obj_ptr = DATA_BASE + off + 0x200
    uc.mem_write(obj_ptr, b"\x00" * 256)

    esp0 = 0x00800000 + 0x40000 - 0x200
    ret_addr = esp0

    def run_call(target_addr, frame_words):
        frame = [struct.pack("<I", 0xCCCCCCCC)] + [struct.pack("<I", w) for w in frame_words]
        uc.mem_write(ret_addr, b"".join(frame))
        done["hit"] = False
        uc.reg_write(UC_X86_REG_ESP, ret_addr)
        uc.reg_write(UC_X86_REG_EBP, ret_addr)
        uc.reg_write(UC_X86_REG_EIP, target_addr)
        try:
            uc.emu_start(target_addr, 0, timeout=10 * UC_SECOND_SCALE)
        except UcError as e:
            if not done["hit"]:
                raise

    frame_words = []
    di = 0
    if auto_this and sym.startswith("_ZN"):
        # 先调用构造函数（C1）初始化对象
        m = re.match(r"^_ZN(\d+)(.*)C[12]Ev$", sym)
        if m:
            pass  # 目标本身就是构造函数，不重复构造
        else:
            mm = re.match(r"^_ZN(\d+)", sym)
            if mm:
                cls_len = int(mm.group(1))
                cls_name = sym[mm.end():mm.end() + cls_len]
                ctor = syms.get(f"_ZN{len(cls_name)}{cls_name}C1Ev")
                if ctor:
                    run_call(ctor, [obj_ptr])
        frame_words.append(obj_ptr)
    for a in call_args:
        if a is None:
            frame_words.append(data_ptrs[di])
            di += 1
        else:
            frame_words.append(a)
    run_call(target, frame_words)

    if os.environ.get("UNI_PRINT_RET"):
        print("RET", hex(uc.reg_read(UC_X86_REG_EAX)))
    for d, ptr in zip(datas, data_ptrs):
        print(bytes(uc.mem_read(ptr, len(d))).hex())
    for a in reads:
        parts = a.split(":", 2)
        if len(parts) != 3:
            raise ValueError("bad read arg: %s" % a)
        print("READ %s %s" % (parts[1], bytes(uc.mem_read(int(parts[1], 16), int(parts[2], 16))).hex()))
    if show_ret:
        eax = uc.reg_read(UC_X86_REG_EAX) & 0xFFFFFFFF
        edx = uc.reg_read(UC_X86_REG_EDX) & 0xFFFFFFFF
        print(f"RET=eax:{eax:#010x} edx:{edx:#010x} (64bit:{((edx << 32) | eax):#018x})")


if __name__ == "__main__":
    main()
