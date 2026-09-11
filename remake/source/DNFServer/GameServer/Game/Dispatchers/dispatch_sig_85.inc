// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08206fea orig/ours=124/128 ndiff-blocks=22
//   帧差 sub $24 vs $38 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %ebx | sub    $0x24,%esp | movl   $&_ZZN28Dispatcher_ConnectP2PAssault12dispatch_sigEP5CUserR9PacketBufE19__PRETTY_FUNCTION__,0x4(%esp)
int Dispatcher_ConnectP2PAssault::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa1b0,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    char count = -1;                                       // -0xd（local_19[8]）
    if (!packet.get_byte(count))
    {
        return LineFunc(0xa1b4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (count > 8 || count < 0)
    {
        return LineFunc(0xa1b8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [g01 2026-09-11 · 新范式 P-g01-a 实证（探针 z2.cpp，444+i486 本文档档位）]
    //   ORIG [44:47] `movl $0x0,-0x15(%ebp); movl $0x0,-0x11(%ebp)`（两条直写 disp）⇔
    //   聚合零初始化 `= {0}`（探针 H/I/J/M 全产此形）；原 cast 写
    //   `*(unsigned int*)(slots+N)=0` 产 `lea+add+movl (%eax)`（5 条，多 3 条 + 间接 store）
    //   ⇒ 4.4 不折叠数组退化指针的常地址。
    char slots[8] = {0};                                   // -0x15..-0xe（base -0x15）
    char team = -1;                                        // -0x16（local_1a）
    for (int i = 0; i < (int)count; i++)
    {
        if (!packet.get_byte(team))
        {
            return LineFunc(0xa1bf,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (team > 7 || team < 0)
        {
            return LineFunc(0xa1c2,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(slots[(int)team]))
        {
            return LineFunc(0xa1c5,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
    }
    // ORIG [109:111]：`xor $0x1,%eax; test %al,%al`（无 `movb %al,-0xd(%ebp)`/
    //   `xorb $1,-0xd(%ebp)` 槽往返）⇒ 裸 `!` 形（旧 X2 具名形多 1 槽 + 2 条）。
    // [g01 2026-09-11 · P0 槽律] ORIG 无 mgr 具名槽：`GetInstanceAssaultMgr()` 结果
    //   直传 (%esp)（receiver 最后求值），且 `(int)count` 先入 callee-saved %ebx
    //   跨 ctor/调用保活 ⇒ 原码为内联链式，无具名 receiver 缓存（P160）。
    if (!pvp_assault::GetInstanceAssaultMgr()->OnConnectP2P(user, slots, (int)count))
    {
        return LineFunc(0xa1cb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}