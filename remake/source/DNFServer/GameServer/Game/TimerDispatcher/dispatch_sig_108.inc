int TimerRevivalCharac::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x08638e78：单出口 result(%ebx) 链，三段 if/else-if；
    // 0x08638ed8 GetRevivalTime 比较对象为 p2（0x10(%ebp)），非 p3；
    // PacketGuard 于 else 块内构造+dtor（0x08638f39 call D1）后 result=1。
    // 技巧2：result 驻 %ebx（ORIG 0x08638ea5 `mov $0x0,%ebx`×3 / 0x08638f61
    // `mov %ebx,%eax` 单出口），PacketGuard 回 -0x18、帧 0x20。
    register int result;   // [b28d-30] 纯 register 存储建议（P231/P248：值跨 call 保活、4.4 不自升格、ORIG 目标 %ebx）
    CUser* user = G_CGameManager()->getUser(p1, 2);   // ORIG 第二实参为常量 2
    if (user == 0) {
        result = 0;
    } else if (user->getCurCharacR() == 0) {
        result = 0;
    } else if (user->GetRevivalTime() != p2) {
        result = 0;
    } else {
        // ORIG 0x08638f34：result=1 在 PacketGuard dtor(0x08638f39) 之前（else 块内）。
        PacketGuard packet;
        packet.put_header(0, 0xe4);
        packet.finalize(true);
        user->Send(packet);
        result = 1;
    }
    return result;
