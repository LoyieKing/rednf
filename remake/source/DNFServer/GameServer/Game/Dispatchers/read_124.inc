int Dispatcher_InformNotice_2nd::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_INFORM_NOTICE_2ND& req = *reinterpret_cast<MSG_INFORM_NOTICE_2ND*>(&msg);
    if (!packet.get_int(reinterpret_cast<unsigned int&>(req.m_noticeFlag)))
    {
        return LineFunc(0x59b8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
