int Dispatcher_TitleBookOtherUser::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_TITLEBOOK_OTHERUSER& req = *reinterpret_cast<MSG_TITLEBOOK_OTHERUSER*>(&msg);
    if (!packet.get_short(req.m_targetIdx))
    {
        return LineFunc(0x4244, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
