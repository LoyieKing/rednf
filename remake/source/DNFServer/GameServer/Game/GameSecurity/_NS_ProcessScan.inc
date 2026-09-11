// ===========================================================================
// _NS_ProcessScan  ORIG T 0x8274523：读 3 字节（b%10 + c%10*10 编码值）→
// HP_GetHackType(a) → SetETC → addServerHackCnt(user, type, 1, etc, 0)。
// 各步读取失败依次返回 1/2/3。
// ===========================================================================
// [b28d-24] DIFF 残24: 控制流/帧差异（P167 待复核）。ORIG 0x08274523
int _NS_ProcessScan(CUser* user, PacketBuf& packet)
{
    char a = 0, b = 0, c = 0;
    if (!packet.get_byte(a))
        return 1;
    if (!packet.get_byte(b))
        return 2;
    if (!packet.get_byte(c))
        return 3;

    unsigned int value = static_cast<int>(b % 10) + static_cast<int>(c % 10) * 10;
    // [b28d-solo-GameSecurity] ORIG 无 etc 具名槽（P0）：SetETC 结果 `mov %eax,%ebx`
    // 直接升格 callee-saved 跨 getHackAnalyzer 保活 ⇒ 源码把 SetETC 写成第 4 实参
    // 内联表达式；具名 int etc 会多一槽 + push %esi + 全帧深 4。
    WongWork::ENUM_HACKTYPE hackType = static_cast<WongWork::ENUM_HACKTYPE>(
        HP_GetHackType(static_cast<unsigned char>(a)));
    user->getHackAnalyzer()->addServerHackCnt(
        user, hackType, 1,
        user->SetETC(static_cast<short>(hackType), value), 0);
    return 0;
}