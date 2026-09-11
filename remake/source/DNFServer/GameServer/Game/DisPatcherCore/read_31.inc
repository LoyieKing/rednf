// ORIG 0x081bed4a T _ZN19DisPatcher_BuySkill4readER9PacketBufR8MSG_BASE
// [b28d-29] AE：169=169 全等（同上重定位归一）  (ORIG 0x081bed4a insn 169->170)
// 报告：docs/class_func_reports/DisPatcher_BuySkill/read.md（汇编+反编译 C 全读）
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 169 / ours 169 insn 全等（AE 口径，重定位归一）。
int DisPatcher_BuySkill::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_BUY_SKILL& m = *reinterpret_cast<MSG_BUY_SKILL*>(&msg);
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_treeKind)))  // +0x0d（signed char→char& 适配）
    {
        return LineFunc(0x37b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // [b28d-solo-DisPatcherCore] P167 真 bug：ORIG 调用序列 [c,c,h,c,c] ⇒ m_count /
    // m_refundFlag / m_level 均绑 `get_byte(char&)`（ERc），仅 m_skillIdx 为 ERh。
    if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_count)))   // +0x0e
    {
        return LineFunc(0x37e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // ORIG test/`jg`+cmp $0x46/`jle`（read.md +0x9d/+0xd1）：比较对象为
    // **signed char**（movzbl 后按 al 符号判定），必须显式 (char) 转换。
    if (!((char)m.m_count > 0))
    {
        return LineFunc(0x381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!((char)m.m_count < 0x47))
    {
        return LineFunc(0x383,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    for (int i = 0; i < (char)m.m_count; ++i)
    {
        if (!packet.get_byte(m.m_entries[i].m_skillIdx))      // +0x0f+3i
        {
            return LineFunc(0x388,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_entries[i].m_refundFlag)))  // +0x11+3i
        {
            return LineFunc(0x38a,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (m.m_entries[i].m_refundFlag != 0 && m.m_entries[i].m_refundFlag != 1)
        {
            return LineFunc(0x38d,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if (!packet.get_byte(*reinterpret_cast<char*>(&m.m_entries[i].m_level)))  // +0x10+3i
        {
            return LineFunc(0x38f,
                            __PRETTY_FUNCTION__,
                            0, 0);
        }
        if ((char)m.m_entries[i].m_level < 1)
        {
            m.m_entries[i].m_level = 1;
        }
    }
    return 0;
// [b28d-29] N82：帧 0x40 vs 0x60 + 局部对象槽平移（-0x20 vs -0x38），调用点全等  (ORIG 0x081bf468 insn 311->302)
}
