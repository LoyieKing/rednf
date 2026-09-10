CNexonMsgTable::CNexonMsgTable()   // ORIG T 0x0858001c
{
    // P2 反向：ORIG 尾测 `mov -0xc,%eax; cmp $0x1b,%eax; setbe`（无符号序）
    // ⇒ 循环变量 unsigned（int 生 cmpl mem+setle）。
    // N 封顶：ORIG 尾测 `mov -0xc,%eax; cmp $0x1b,%eax; setbe` 寄存器装载形；
    // 4.4 对 unsigned 局部恒折 `cmpl $imm,mem`（for/do-while/<=/< 三形实测
    // 69/68/69，do-while 反少前置 jmp 岛），源形不可择。
    for (unsigned int i = 0; i < 0x1c; ++i)
        m_messages.insert(std::make_pair(msgs[i].key, msgs[i].str));
