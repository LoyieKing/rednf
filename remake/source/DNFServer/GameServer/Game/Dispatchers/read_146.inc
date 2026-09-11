int Dispatcher_DeleteToBlackList::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cde36：int@0x10（len）→ str@0x14（max 0x1e）
    MSG_DELETE_TO_BLACKLIST& mref =
        reinterpret_cast<MSG_DELETE_TO_BLACKLIST&>(msg);
    if (!packet.get_int(mref.m_nameLen))
        return LineFunc(0x26ce, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(mref.m_characName, 0x1e, mref.m_nameLen))
        return LineFunc(0x26d1, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}