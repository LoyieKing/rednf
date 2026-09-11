// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x08226bda orig/ours=123/93 ndiff-blocks=10
//   帧差 sub $40 vs $48 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: push   %esi | push   %ebx | sub    $0x40,%esp
// [SweepDispatchers 2026-09-09] 真缺口修复（93→123 = ORIG 123；p167 13/13 CLEAR）：
//   ① ORIG 0x08226d36 只调一次 trim 且其返回值直接作 strncmp 第 1 参
//      （`call trim; mov %eax,(%esp)`）——旧码 `trim(name); strncmp(name,…)` 双求值；
//   ② nameLen=0 先于 name 清零（ORIG 0x08226c3a/0x08226c41）+ name 改 `= {0}`。
int Dispatcher_QueryCharacInfoMailbox::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() < 3 || user->getCurCharacR() == 0)
        return LineFunc(0xe289, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x08226c3a/0x08226c41：先 nameLen=0（-0xc）再 name 清零（-0x2a）。
    int nameLen = 0;                                 // -0xc
    char name[0x1e] = {0};                           // -0x2a（ORIG 内联慢路，N78）
    if (!packet.get_int(nameLen))
        return LineFunc(0xe28e, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(name, 0x1e, nameLen))
        return LineFunc(0xe28f, __PRETTY_FUNCTION__, 0, 0);
    // ORIG 0x08226d29/0x08226d36/0x08226d47：trim 只调一次且返回值直接作 strncmp 第 1 参
    // （trim 返回 char*，ORIG `call trim; mov %eax,(%esp)`）；curName 先入 %ebx（右→左实参序）。
    if (strncmp(trim(name), user->getCurCharacName(), 0x1d) == 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x145, 7);
        return 0;
    }
    DB_ReqQueryCharacInfoMailbox::makeRequest((int)user->GetUID(), name);
    // [G1-Dispatchers] ORIG 尾部 0x08226d5x 为 `mov $0x0,%eax`（return 0）；
    // 原部件缺该 return（函数体由聚合器 } 闭合）⇒ 补齐，兼修未定义返回值。
    return 0;
}