int Dispatcher_RequestMemberEnter::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cb780：int@0x10（len）→ str@0x14（max 0x1e）
    MSG_REQUEST_MEMBER_ENTER& m = reinterpret_cast<MSG_REQUEST_MEMBER_ENTER&>(msg);
    if (!packet.get_int(m.m_nameLen))
        return LineFunc(0x2169, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_str(m.m_characName, 0x1e, m.m_nameLen))
        return LineFunc(0x216c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
