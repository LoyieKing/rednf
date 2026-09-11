// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08205f68 orig/ours=120/90 ndiff-blocks=10
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
// [SweepDispatchers 2026-09-09] 形态收敛（90→120 = ORIG 120；p167 12/12 CLEAR）：
//   ORIG 0x08205fc8 先 `len=0` 再 0x08205fcf name 清零（4.1 内联对齐剥离慢路）；
//   旧码 memset call + 顺序颠倒。改 `int len = 0; char name[0x1e] = {0};` 后指令数全等。
int Dispatcher_QueryCharacInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2 || user->getCurCharacR() == 0)
    {
        return LineFunc(0x9f60,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG 0x08205fc8/0x08205fcf：先 len=0（-0xc）再 name 清零（-0x2a，4.1 内联
    // 对齐剥离慢路；4.4 以 = {0} 表达，形态差归 N78）。
    int len = 0;                                           // -0xc
    char name[0x1e] = {0};                                 // -0x2a
    if (!packet.get_int(len))
    {
        return LineFunc(0x9f65,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(name, 0x1e, len))
    {
        return LineFunc(0x9f66,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (strncmp(name, user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x8b, 7);
        return 0;
    }
    DB_ReqQueryCharacInfo::makeRequest((int)user->GetUID(), name);
    return 0;
}