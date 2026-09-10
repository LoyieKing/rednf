int DisPatcher_ChangeSkill::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081...：无指针局部，三处均 `mov 0x10(%ebp),%eax; add $0xd/e/f`（帧 0x18）。
    if (!packet.get_byte(reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_head[0xd]))   // +0x0d
    {
        return LineFunc(0x6f4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_curSlot)))              // +0x0e
    {
        return LineFunc(0x6f6,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(
            &reinterpret_cast<MSG_CHANGE_SKILLSLOT*>(&msg)->m_newSlot)))               // +0x0f
    {
        return LineFunc(0x6f8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
