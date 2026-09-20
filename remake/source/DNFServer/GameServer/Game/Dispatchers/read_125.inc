int Dispatcher_MemberEnterReply::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MEMBER_ENTER_REPLY& req = *reinterpret_cast<MSG_MEMBER_ENTER_REPLY*>(&msg);
    if (!packet.get_byte(req.m_reply))
    {
        return LineFunc(0x21d4, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
