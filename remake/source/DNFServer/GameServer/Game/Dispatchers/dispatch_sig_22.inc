int Dispatcher_TraceError::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // [FL1 2026-09-10] ORIG 0x08204b0f/0x08204b13 直证：序言 `push %ebx; sub $0x34`
    //   后直接 `lea -0x19(%ebp),%eax` 传 &errorType —— **无清零**（P171/P0：
    //   原码未写的初始化不得补），故删 `= 0`。errorCode 同理（-0x1a 无 movb $0）。
    char errorType;
    if (!packet.get_byte(errorType))
    {
        return LineFunc(0x9c74,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [FL1 2026-09-10 负结论] ORIG 0x08204b55 为**单次载入 + 两路 cmp**：
    //   `movzbl -0x19; movsbl %al,%eax; cmp $0x2,%eax; je L2; cmp $0x3,%eax; jne end`。
    //   三形实测：① 裸 if/else-if → ours 63（`cmp $0x2,%al` 字节比较、两支各自重载）；
    //   ② 显式 (int) cast → 无效（4.4 仍折 %al）；③ `switch((int)…)` → 64（多 `jmp default`）。
    //   ⇒ 4.4 恒「一载入一比较」，属 N18b 控制流折叠，封顶。
    if (errorType == 2)
    {
        char errorCode;
        if (!packet.get_byte(errorCode))
        {
            return LineFunc(0x9cb3,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        // [FL1] P227 全表达式临时链：ORIG 0x08204bd2 段 `movzbl -0x1a; movsbl %al,%ebx`
        //   （ebx 保活）→ ctor → `mov %ebx,0x8(%esp)`（ctor 之后才压实参）⇒ 保留此形。
        cMyTrace(__PRETTY_FUNCTION__,
                 0x9cb5, 0)
            ("KILLBOSS2 client - error type(%d)", (int)errorCode);
    }
    else if (errorType == 3)
    {
        return LineFunc(0x9c85,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
