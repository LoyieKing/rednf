int Dispatcher_ChangeEmotion::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_CHANGE_EMOTION& req = *reinterpret_cast<MSG_CHANGE_EMOTION*>(&msg);
    if (!packet.get_short(req.m_emotionIdx))
    {
        return LineFunc(0x2e3d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
