int Dispatcher_InformNotice::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_INFORM_NOTICE& req = *reinterpret_cast<MSG_INFORM_NOTICE*>(&msg);
    if (!packet.get_int(reinterpret_cast<unsigned int&>(req.m_noticeFlag)))
    {
        return LineFunc(0x597d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
