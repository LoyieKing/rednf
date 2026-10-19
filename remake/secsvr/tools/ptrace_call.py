#!/usr/bin/env python3
"""ptrace 调用器：在 secagent 进程内调用任意符号函数（32 位，cdecl，this 作首参）。

用法：python3 ptrace_call.py <symbol> <arg>...
  数据参数：hex 字符串（分配内存传指针，调用后读回内容）
  整数参数：int:N 或 0xADDR（直接传值）
例：python3 ptrace_call.py _ZN5CIdea4IdeaEPhbS0_j \
      000102030405060708090a0b0c0d0e0f int:1 0011223344556677 int:8
输出：每个指针参数调用后的内容 hex。
"""
import ctypes
import os
import struct
import sys
import time

BIN = "/mnt/d/Docs/my_sources/dnf_workspace/dnf_installer/build/dnf_data/home/template/neople/secsvr/zergsvr/secagent"

libc = ctypes.CDLL("libc.so.6", use_errno=True)
libc.ptrace.restype = ctypes.c_long

PTRACE_TRACEME = 0
PTRACE_PEEKDATA = 2
PTRACE_POKEDATA = 5
PTRACE_CONT = 7
PTRACE_KILL = 8
PTRACE_GETREGS = 12
PTRACE_SETREGS = 13
PTRACE_ATTACH = 16
PTRACE_SETOPTIONS = 0x4200
PTRACE_O_TRACEEXEC = 0x10
HOLD_SO = "/tmp/hold.so"


class UserRegs(ctypes.Structure):
    _fields_ = [
        ("ebx", ctypes.c_uint32), ("ecx", ctypes.c_uint32),
        ("edx", ctypes.c_uint32), ("esi", ctypes.c_uint32),
        ("edi", ctypes.c_uint32), ("ebp", ctypes.c_uint32),
        ("eax", ctypes.c_uint32), ("xds", ctypes.c_uint32),
        ("xes", ctypes.c_uint32), ("xfs", ctypes.c_uint32),
        ("xgs", ctypes.c_uint32), ("orig_eax", ctypes.c_uint32),
        ("eip", ctypes.c_uint32), ("xcs", ctypes.c_uint32),
        ("eflags", ctypes.c_uint32), ("esp", ctypes.c_uint32),
        ("xss", ctypes.c_uint32),
    ]


def ptrace(req, pid, addr=0, data=0):
    if isinstance(data, ctypes.Structure):
        data = ctypes.byref(data)
    r = libc.ptrace(req, pid, addr, data)
    if r == -1:
        err = ctypes.get_errno()
        raise OSError(err, os.strerror(err))
    return r


def symbol_addr(name):
    out = os.popen(f"nm '{BIN}'").read()
    for line in out.splitlines():
        parts = line.split(None, 2)
        if len(parts) >= 3 and parts[2] == name:
            return int(parts[0], 16)
    raise KeyError(f"symbol not found: {name}")


def is_method(sym):
    out = os.popen(f"nm -C '{BIN}' | grep ' {sym}$'").read()
    return "::" in out.split(" ", 2)[-1] if out.strip() else False


def main():
    sym = sys.argv[1]
    args = sys.argv[2:]
    target = symbol_addr(sym)
    method = is_method(sym)

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

    data_total = sum(len(d) + 16 for d in datas) + 0x400
    STACK_SIZE = 0x10000
    DATA_SIZE = data_total + 0x1000

    pid = os.fork()
    if pid == 0:
        os.environ["LD_PRELOAD"] = HOLD_SO
        os.execv(BIN, [BIN])
        os._exit(127)

    # 等 child 完成 exec 并进入 hold.so 的 constructor（用户态）
    for _ in range(200):
        if not os.path.exists(f"/proc/{pid}/maps"):
            break
        if "hold.so" in open(f"/proc/{pid}/maps").read():
            break
        time.sleep(0.01)
    time.sleep(0.05)
    ptrace(PTRACE_ATTACH, pid, 0, 0)
    _, status = os.waitpid(pid, 0)
    if not os.WIFSTOPPED(status):
        raise RuntimeError(f"child not stopped after attach: {status}")
    regs0 = UserRegs()
    ptrace(PTRACE_GETREGS, pid, 0, ctypes.byref(regs0))
    if regs0.eip == 0:
        raise RuntimeError("attached in kernel mode")

    maps = open(f"/proc/{pid}/maps").read()
    region = None
    for line in maps.splitlines():
        rng, perms = line.split()[:2]
        lo, hi = (int(x, 16) for x in rng.split("-"))
        if "w" in perms and (hi - lo) >= STACK_SIZE + DATA_SIZE:
            region = (lo, hi)
            break
    if region is None:
        raise RuntimeError("no usable memory region")
    base = region[0]
    stack_top = base + STACK_SIZE - 16
    data_base = base + STACK_SIZE + 0x100

    data_ptrs = []
    off = 0
    for d in datas:
        ptr = data_base + off
        data_ptrs.append(ptr)
        chunk = d + b"\x00" * 16
        for i in range(0, len(chunk), 4):
            word = struct.unpack("<I", chunk[i:i+4].ljust(4, b"\x00"))[0]
            ptrace(PTRACE_POKEDATA, pid, ptr + i, word)
        off += len(d) + 16

    obj_ptr = data_base + off + 0x200
    ptrace(PTRACE_POKEDATA, pid, obj_ptr, 0)
    ptrace(PTRACE_POKEDATA, pid, obj_ptr + 4, 0)

    ret_addr = stack_top
    frame = [struct.pack("<I", ret_addr)]
    di = 0
    if method:
        frame.append(struct.pack("<I", obj_ptr))  # this
    for a in call_args:
        if a is None:
            frame.append(struct.pack("<I", data_ptrs[di]))
            di += 1
        else:
            frame.append(struct.pack("<I", a))
    frame_bytes = b"".join(frame)
    for i in range(0, len(frame_bytes), 4):
        word = struct.unpack("<I", frame_bytes[i:i+4].ljust(4, b"\x00"))[0]
        ptrace(PTRACE_POKEDATA, pid, ret_addr + 4 + i, word)
    ptrace(PTRACE_POKEDATA, pid, ret_addr, 0xCCCCCCCC)

    esp = ret_addr
    regs = UserRegs()
    ptrace(PTRACE_GETREGS, pid, 0, regs)
    regs.eip = target
    regs.esp = esp
    regs.ebp = esp
    ptrace(PTRACE_SETREGS, pid, 0, regs)

    ptrace(PTRACE_CONT, pid, 0, 0)
    try:
        _, status = os.waitpid(pid, 0)
    except ChildProcessError:
        raise RuntimeError("child exited during call")
    if not os.WIFSTOPPED(status):
        raise RuntimeError(f"target returned without int3: {status}")

    results = []
    for d, ptr in zip(datas, data_ptrs):
        out = []
        for i in range(0, len(d), 4):
            word = ptrace(PTRACE_PEEKDATA, pid, ptr + i, 0) & 0xFFFFFFFF
            out.append(struct.pack("<I", word))
        results.append(b"".join(out)[:len(d)])
    ptrace(PTRACE_KILL, pid, 0, 0)
    os.waitpid(pid, 0)

    for r in results:
        print(r.hex())


if __name__ == "__main__":
    main()
