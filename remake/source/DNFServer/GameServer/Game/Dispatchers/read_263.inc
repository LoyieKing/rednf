int Dispatcher_Enchant_3rdChronicleItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081d3630（报告 Dispatcher_Enchant_3rdChronicleItem/read.md）：
    // 无具名引用槽——每处 `mov 0x10(%ebp),%eax` 直读参数（帧 0x18）。
    // 具名引用中转变量=4.4 多 1 个 4B 槽⇒删中转改 cast 内联（技巧36/37）。
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(
            &reinterpret_cast<MSG_ENCHANT&>(msg).m_13)))
    {
        return LineFunc(
            0x3158,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(
            &reinterpret_cast<MSG_ENCHANT&>(msg).m_15)))
    {
        return LineFunc(
            0x315a,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(
            &reinterpret_cast<MSG_ENCHANT&>(msg).m_0d)))
    {
        return LineFunc(
            0x315d,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_int(*reinterpret_cast<unsigned long*>(
            &reinterpret_cast<MSG_ENCHANT&>(msg).m_0f)))
    {
        return LineFunc(
            0x315f,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_byte(reinterpret_cast<MSG_ENCHANT&>(msg).m_19))
    {
        return LineFunc(
            0x3162,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}