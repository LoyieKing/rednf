// ORIG 0x081c4396 T _ZN23DisPatcher_SetPartyInfo4readER9PacketBufR8MSG_BASE
// （docs/class_func_reports/DisPatcher_SetPartyInfo/read.md 汇编+反编译 C 全文转写）：
// get_byte(titleIndex) → titleIndex==0 时 get_int(+0x0e) 并 get_str(title,0x20,+0x0e)
// → get_byte(userMax)/get_short(dungIndex)/get_byte(dungDiffi)；任一失败 LineFunc
// 0x10d9/0x10dd/0x10e0/0x10e2/0x10e5/0x10e6，全成功返回 0。
int DisPatcher_SetPartyInfo::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_PARTY_SET_INFO& req = *reinterpret_cast<MSG_PARTY_SET_INFO*>(&msg);
    if (!packet.get_byte(req.m_titleIndex))
    {
        return LineFunc(0x10d9, __PRETTY_FUNCTION__, 0, 0);
    }
    if (req.m_titleIndex == 0)
    {
        if (!packet.get_int(req.m_titleLen))
        {
            return LineFunc(0x10dd, __PRETTY_FUNCTION__, 0, 0);
        }
        if (!packet.get_str(req.m_title, 0x20, req.m_titleLen))
        {
            return LineFunc(0x10e0, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    if (!packet.get_byte(req.m_userMax))
    {
        return LineFunc(0x10e2, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_short(req.m_dungIndex))
    {
        return LineFunc(0x10e5, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_byte(req.m_dungDiffi))
    {
        return LineFunc(0x10e6, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
