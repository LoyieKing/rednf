int Dispatcher_UseRenameCard::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_USE_RENAME_CARD& req = *reinterpret_cast<MSG_USE_RENAME_CARD*>(&msg);
    if (!packet.get_short(reinterpret_cast<short&>(req.m_0e)))
    {
        return LineFunc(0x62f3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
