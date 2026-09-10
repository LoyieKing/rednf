int Dispatcher_ItemHyperLinkMessage::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ITEM_HYPER_LINK_MESSAGE& m =
        reinterpret_cast<MSG_ITEM_HYPER_LINK_MESSAGE&>(msg);
    if (!packet.get_byte(m.m_typeD))
        return LineFunc(0x4765,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(m.m_itemSpaceE))
        return LineFunc(0x4768,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_slot10))
        return LineFunc(0x476b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_textLen14))
        return LineFunc(0x476e,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_str(m.m_text18, 0x100, m.m_textLen14))
        return LineFunc(0x4771,
                        __PRETTY_FUNCTION__,
                        0, 0);

    if (m.m_typeD == 1 || m.m_typeD == 0x26)
    {
        if (!packet.get_int(m.m_nameLen118))
            return LineFunc(0x4785,
                            __PRETTY_FUNCTION__,
                            0, 0);
        if (!packet.get_str(m.m_name11c, 0x1e, m.m_nameLen118))
            return LineFunc(0x4788,
                            __PRETTY_FUNCTION__,
                            0, 0);
        // ORIG 0x081d98c4：`mov -0x10(%ebp),%eax; add $0x13a,%eax; mov %eax,0x4(%esp)`
        // —— 字段地址直传（无 field13a 临时槽、取完无回写 store，P0）⇒ 删中转。
        if (!packet.get_byte(m.m_field13a))
            return LineFunc(0x478c,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }

    if (!packet.get_byte(m.m_linkCount13b))
        return LineFunc(0x4792,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_linkCount13b == 0 || m.m_linkCount13b > 3)
        return LineFunc(0x4795,
                        __PRETTY_FUNCTION__,
                        0, 0);
    // ORIG 0x081d98c4 循环体：`movl $0x0,-0xc(%ebp)`（**int** 槽）+ 尾测
    // `movzbl 0x13b(%eax),%eax; movzbl %al,%eax; cmp -0xc(%ebp),%eax; setg`
    // ⇒ 索引是 int（T1：32 位 cmp/setg 有符号），uchar 会退化成 addb/cmpb。
    for (int i = 0; i < m.m_linkCount13b; ++i)
    {
        if (!packet.get_binary((char*)m.m_links13c[i], 0x68))
            return LineFunc(0x479a,
                            __PRETTY_FUNCTION__,
                            0, 0);
    }
    return 0;
