bool DbmwClient::Regist()
{
    // ORIG @0x081209ba 整读（range 0x081209ba..0x081209f9，63 字节）：
    //   +0x06 mov this,(%esp); call RegistToMiddleware
    //   +0x11 xor $0x1,%eax; test %al,%al; je +0x1f        ← 成功跳下一段
    //   +0x18 mov $0x0,%eax; jmp +0x3d(end)                 ← 失败返回 0
    //   +0x1f mov this,(%esp); call ReceiveRegistAck
    //   +0x2a xor $0x1,%eax; test %al,%al; je +0x38
    //   +0x31 mov $0x0,%eax; jmp +0x3d
    //   +0x38 mov $0x1,%eax
    //   +0x3d leave; ret
    // 关键：原写法 `if (A()) return B();` 把 B 的结果**透传**（`call B; leave; ret`），
    // ORIG 实际是**双臂各自 xor 取反 + 常量返回 + 单一 epilogue**（gap 缺的 7 条
    // 就是第二个 xor/test/je/mov/jmp 段）。故必须写成两次"取反判断 + 常量返回"，
    // 并用 goto 把两个失败出口汇到同一个 `mov $0x0,%eax; jmp end`（技巧 23）。
    // 技巧 1（在册，比 X1 register 形更短）：被调本身返回 **bool** 时，
    // `if (!boolcall())` 在 4.4 直接产 ORIG 形 `call; xor $0x1,%eax;
    // test %al,%al; je <下一段>` + `mov $0x0,%eax; jmp end` 落点岛，
    // 无需 register asm 绑定（X1 形是给 int 返回准备的，这里用反而把
    // 分支极性做成 jne，实测 ndiff 9→11 变差）。
    if (!RegistToMiddleware())
        return false;
    if (!ReceiveRegistAck())
        return false;
    return true;
