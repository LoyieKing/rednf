#!/usr/bin/env python3
"""unicorn 调用器（还原 .so 侧）：加载本地编译的 32 位共享库并调用符号函数。

与 uni_call.py 同协议：--plain 关闭 this 构造；--ret 打印 EAX；
参数 int:N / 0xADDR / hex 数据（分配 guest 内存传指针，调用后读回）。
libc 处理子集与 uni_call.py 一致（malloc/free/string/ctype/mktime/printf 族）。
用法：
  python3 so_call.py [--ret] <mangled symbol> <arg>...
"""
import struct
import sys
import os
import time as htime

from unicorn import *
from unicorn.x86_const import *

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from uni_call import parse_elf, load_segments, parse_plt

SO = "/tmp/libzenlib_secagent.so"
BASE = 0x40000000


def load_symtab(data, sections):
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
            out[nm] = BASE + st_value
    return out


def apply_relocs(uc, data, sections, syms):
    """应用 .rel.dyn / .rel.plt 重定位（R_386_32 / PC32 / GLOB_DAT /
    JUMP_SLOT / RELATIVE），把内部符号写入 GOT/数据区。"""
    for name, sh in sections.items():
        sh_type = sh[1]
        if sh_type not in (4, 9):
            continue
        r_off, r_size = sh[4], sh[5]
        for i in range(r_size // 8):
            r_offset, r_info = struct.unpack_from("<II", data, r_off + i * 8)
            rtype = r_info & 0xff
            sym_idx = r_info >> 8
            if rtype in (1, 2, 6, 7):
                sym_name = None
                dynsym = sections.get(".dynsym")
                if dynsym is None:
                    continue
                st_name, st_value, st_size, st_info, st_other, st_shndx = \
                    struct.unpack_from("<IIIBBH", data, dynsym[4] + sym_idx * 16)
                str_off = sections[".dynstr"][4]
                strtab = data[str_off:]
                sym_name = strtab[st_name:strtab.find(b"\x00", st_name)].decode() if st_name else ""
                base = syms.get(sym_name, 0)
                if rtype in (1, 2):
                    addend = struct.unpack("<I", uc.mem_read(BASE + r_offset, 4))[0]
                    if rtype == 1:
                        base = (base + addend) & 0xFFFFFFFF
                    else:  # R_386_PC32: S + A - P
                        base = (base + addend - (BASE + r_offset)) & 0xFFFFFFFF
                uc.mem_write(BASE + r_offset, struct.pack("<I", base))
            elif rtype == 8:    # R_386_RELATIVE: B + A
                addend = struct.unpack("<I", uc.mem_read(BASE + r_offset, 4))[0]
                uc.mem_write(BASE + r_offset, struct.pack("<I", (BASE + addend) & 0xFFFFFFFF))


def main():
    os.environ.setdefault("TZ", "UTC")
    htime.tzset()
    args0 = sys.argv[1:]
    auto_this = True
    show_ret = False
    if args0 and args0[0] == "--plain":
        auto_this = False
        args0 = args0[1:]
    if args0 and args0[0] == "--ret":
        show_ret = True
        args0 = args0[1:]
    sym = args0[0]
    args = args0[1:]
    data = open(SO, "rb").read()
    phdrs, sections = parse_elf(data)

    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    # ET_DYN 的 vaddr 是文件内偏移，整体加载到 BASE
    shifted = [(p_type, p_offset, BASE + p_vaddr, p_filesz, p_memsz, p_flags)
               for (p_type, p_offset, p_vaddr, p_filesz, p_memsz, p_flags) in phdrs]
    load_segments(uc, data, shifted)
    syms = load_symtab(data, sections)
    apply_relocs(uc, data, sections, syms)
    plt_map = parse_plt(data, sections)
    plt_map = {BASE + k: v for k, v in plt_map.items()}

    uc.mem_map(0x00500000, 0x100000)   # 宿主堆
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
        a = [struct.unpack("<I", uc.mem_read(esp + 4 + i * 4, 4))[0] for i in range(4)]

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

        if name in ("malloc", "_Znwj", "_Znam", "_Znwm", "_Znaj", "operator new"):
            result = host_malloc(a[0])
        elif name in ("free", "_ZdlPv", "_ZdaPv", "operator delete"):
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
            result = len(rd_cstr(a[0]))
        elif name == "strchr":
            s = rd_cstr(a[0])
            c = a[1] & 0xFF
            p = s.find(bytes([c]))
            result = a[0] + p if p >= 0 else 0
        elif name == "strrchr":
            s = rd_cstr(a[0])
            c = a[1] & 0xFF
            p = s.rfind(bytes([c]))
            result = a[0] + p if p >= 0 else 0
        elif name in ("strcpy", "stpcpy"):
            wr_cstr(a[0], rd_cstr(a[1]))
            result = a[0]
        elif name == "strncpy":
            src = rd_cstr(a[1])
            n = a[2]
            b = src[:n]
            if len(b) < n:
                b += b"\x00" * (n - len(b))
            uc.mem_write(a[0], b)
            result = a[0]
        elif name == "strcmp":
            ba = rd_cstr(a[0])
            bb = rd_cstr(a[1])
            result = (ba > bb) - (ba < bb)
        elif name == "strncmp":
            ba = bytes(uc.mem_read(a[0], a[2]))
            bb = bytes(uc.mem_read(a[1], a[2]))
            result = (ba > bb) - (ba < bb)
        elif name in ("strcasecmp", "strncasecmp"):
            def fold(b):
                return bytes((c + 32) if 0x41 <= c <= 0x5A else c for c in b)
            if name == "strcasecmp":
                ba, bb = fold(rd_cstr(a[0])), fold(rd_cstr(a[1]))
            else:
                ba = fold(bytes(uc.mem_read(a[0], a[2])))
                bb = fold(bytes(uc.mem_read(a[1], a[2])))
            result = (ba > bb) - (ba < bb)
        elif name == "isspace":
            c = a[0] & 0xFF
            result = 1 if c in (0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x20) else 0
        elif name == "tolower":
            c = a[0] & 0xFF
            result = c + 32 if 0x41 <= c <= 0x5A else c
        elif name == "toupper":
            c = a[0] & 0xFF
            result = c - 32 if 0x61 <= c <= 0x7A else c
        elif name == "mktime":
            vals = struct.unpack("<9i", uc.mem_read(a[0], 36))
            t = htime.mktime((vals[5] + 1900, vals[4] + 1, vals[3], vals[2],
                              vals[1], vals[0], vals[6], vals[7], vals[8]))
            lt = htime.localtime(t)
            uc.mem_write(a[0] + 24, struct.pack("<i", lt.tm_wday))
            uc.mem_write(a[0] + 28, struct.pack("<i", lt.tm_yday))
            result = t & 0xFFFFFFFF
        else:
            print(f"UNHANDLED LIBC {name}", flush=True)
            result = 0
        uc.reg_write(UC_X86_REG_EAX, result & 0xFFFFFFFF)

    done = {"hit": False}

    def on_code(uc, address, size, user_data):
        if address == ret_addr:
            done["hit"] = True
            uc.emu_stop()
            return
        if address in plt_map:
            name = plt_map[address]
            handle_libc(uc, address, name)
            ret = struct.unpack("<I", uc.mem_read(uc.reg_read(UC_X86_REG_ESP), 4))[0]
            uc.reg_write(UC_X86_REG_ESP, uc.reg_read(UC_X86_REG_ESP) + 4)
            uc.reg_write(UC_X86_REG_EIP, ret)
            return

    uc.hook_add(UC_HOOK_CODE, on_code)

    def on_intr(uc, intno, user_data):
        uc.emu_stop()
        return

    uc.hook_add(UC_HOOK_INTR, on_intr)

    def on_mem_unmapped(uc, access, address, size, value, user_data):
        print(f"MEM_UNMAPPED access={access} addr={address:#x} size={size}", flush=True)
        return False

    uc.hook_add(UC_HOOK_MEM_WRITE_UNMAPPED, on_mem_unmapped)
    uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED, on_mem_unmapped)

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
        uc.reg_write(UC_X86_REG_ESP, ret_addr)
        uc.reg_write(UC_X86_REG_EBP, ret_addr)
        uc.reg_write(UC_X86_REG_EIP, target_addr)
        try:
            done["hit"] = False
            uc.emu_start(target_addr, 0, timeout=10 * UC_SECOND_SCALE)
        except UcError as e:
            if not done["hit"]:
                print("UcError:", e, flush=True)

    frame_words = []
    di = 0
    if auto_this and sym.startswith("_ZN"):
        frame_words.append(obj_ptr)
    for a in call_args:
        if a is None:
            frame_words.append(data_ptrs[di])
            di += 1
        else:
            frame_words.append(a)
    if sym not in syms:
        raise KeyError(f"symbol not found: {sym}")
    run_call(syms[sym], frame_words)

    for d, ptr in zip(datas, data_ptrs):
        print(bytes(uc.mem_read(ptr, len(d))).hex())
    if show_ret:
        eax = uc.reg_read(UC_X86_REG_EAX) & 0xFFFFFFFF
        edx = uc.reg_read(UC_X86_REG_EDX) & 0xFFFFFFFF
        print(f"RET=eax:{eax:#010x} edx:{edx:#010x} (64bit:{((edx << 32) | eax):#018x})")


if __name__ == "__main__":
    main()
