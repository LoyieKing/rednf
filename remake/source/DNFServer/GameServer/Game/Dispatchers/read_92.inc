int Dispatcher_RenameCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    char* pMsg = (char*)&msg;
    // P1：int@0x10 用成员视图折叠（ORIG `mov 0x10(%eax),%eax`）；short/byte 位保持算术形态。
    struct __attribute__((packed)) LenView { char m_head[0x10]; int m_len; };
    if (!packet.get_short(*(short*)(pMsg + 0x0e)))
    {
        return LineFunc(0x24b4,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*(char*)(pMsg + 0x0d)))
    {
        return LineFunc(0x24b7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(*(int*)(pMsg + 0x10)))
    {
        return LineFunc(0x24ba,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_str(pMsg + 0x14, 0xd, ((LenView*)pMsg)->m_len))
    {
        return LineFunc(0x24bd,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
