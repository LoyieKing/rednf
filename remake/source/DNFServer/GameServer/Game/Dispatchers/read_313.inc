// ==== Dispatcher_AvatarOptionChange::read (ORIG 0x081e04f2) ====
int Dispatcher_AvatarOptionChange::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_AVATAR_OPTION& m = *reinterpret_cast<MSG_CHANGE_AVATAR_OPTION*>(&msg);
    if (!packet.get_short(reinterpret_cast<short&>(m.m_consumeSlot)))
        return LineFunc(0x5380,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((short)m.m_consumeSlot < 0 || m.m_consumeSlot > 0x137)
        return LineFunc(0x5383,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_consumeItemIdx))
        return LineFunc(0x5386,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_consumeItemIdx < 0)
        return LineFunc(0x5389,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_short(reinterpret_cast<short&>(m.m_avatarSlot)))
        return LineFunc(0x538c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if ((short)m.m_avatarSlot < 0 || m.m_avatarSlot > 0x68)
        return LineFunc(0x5390,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_int(m.m_equipItemIdx))
        return LineFunc(0x5393,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (m.m_equipItemIdx < 0)
        return LineFunc(0x5396,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (!packet.get_byte(reinterpret_cast<char&>(m.m_option)))
        return LineFunc(0x5399,
                        __PRETTY_FUNCTION__,
                        0, 0);
    if (((unsigned char)m.m_option & 0x80) != 0)
        return LineFunc(0x539c,
                        __PRETTY_FUNCTION__,
                        0, 0);
    return 0;
}