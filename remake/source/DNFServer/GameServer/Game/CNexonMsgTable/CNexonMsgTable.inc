// [b28d-L14 P209附] ORIG 0x0857ffcc：unsigned 局部循环尾测 4.4 恒折
// `cmpl $0x1b,-0xc` 内存直比（ORIG 先 mov 到寄存器再 cmp+setbe）⇒ 封顶。
CNexonMsgTable::CNexonMsgTable()   // ORIG T 0x0858001c
{
    // P2 反向：ORIG 尾测 `mov -0xc,%eax; cmp $0x1b,%eax; setbe`（无符号序）
    // ⇒ 循环变量 unsigned（int 生 cmpl mem+setle）。
    // N 封顶：ORIG 尾测 `mov -0xc(%ebp),%eax; cmp $0x1b,%eax; setbe` 寄存器装载形；
    // 4.4 对 unsigned 局部恒折 `cmpl $imm,mem`（for/do-while/<=/< 三形实测
    // 69/68/69，do-while 反少前置 jmp 岛），源形不可择。
    // [N6S4PosB 复核·仍封 N] ORIG 0x085800a3 `mov -0xc(%ebp),%eax; cmp
    // $0x1b,%eax; setbe %al; test %al,%al; jne`（5 条尾测）+ 0x0858009f
    // `addl $0x1,-0xc(%ebp)`（1 条自增）= 老 GCC「先 load 再 cmp」风格。
    // 本轮探针（45 形）：446/444 后端亦恒折 4 条尾测；volatile 局部（f4）
    // 尾测同 ORIG 形但自增拆成 mov/add/mov 3 条（净 +2）；register 局部（fb）
    // 落 %ebx（`cmpl $27,%ebx`）非内存槽。⇒ 权威口径 70/70 gap+0，唯一残差 =
    // 尾测 `mov -0xc(%ebp),%eax; cmp $0x1b,%eax` 合并形（4.4 恒折单条
    // `cmpl $0x1b,-0xc(%ebp)`），编译器 lowering 不可造 ⇒ 封 N。
    // [P6S6 2026-09-11 复核·仍 N] 再补两形实测（ORIG 0x085800a3 尾测
    //   `mov -0xc(%ebp),%eax; cmp $0x1b,%eax; setbe %al`）：
    //   · `i != 0x1c` → N `cmpl $0x1c,-0xc(%ebp); setne %al`（69/70，blocks=1）
    //   · `i <= 0x1b` → N `cmpl $0x1b,-0xc(%ebp)`（69/70，blocks=1，无 setcc）
    //   四形（</<=/!=/do-while）均在「eax 预装载」处缺 1 条，4.4 恒 mem 直比
    //   ⇒ 确为 4.1↔4.4 后端 lowering 差，源码不可择 ⇒ 维持 N。
    // [Q8 2026-09-11] P310：局部 int + 显式 `(unsigned int)` 比较 ⇒ 复现 ORIG
    // 0x085800a3 两段尾测 `mov -0xc(%ebp),%eax; cmp $0x1b,%eax; setbe`。
    for (int i = 0; (unsigned int)i < 0x1c; ++i)
        m_messages.insert(std::make_pair(msgs[i].key, msgs[i].str));
}