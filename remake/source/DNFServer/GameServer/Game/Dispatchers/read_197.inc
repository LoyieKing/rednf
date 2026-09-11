int Dispatcher_RegisterToBlackList::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cdbc6：int@0x10（len）→ str@0x14（max 0x1e）
    MSG_REGISTER_TO_BLACKLIST& m = reinterpret_cast<MSG_REGISTER_TO_BLACKLIST&>(msg);
    if (!packet.get_int(m.m_nameLen))
        return LineFunc(0x267d, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_characName, 0x1e, m.m_nameLen))
        return LineFunc(0x2680, __PRETTY_FUNCTION__, 0, 0);
    return 0;
}