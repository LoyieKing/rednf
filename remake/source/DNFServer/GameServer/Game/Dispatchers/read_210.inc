int Dispatcher_AbnormalUseStackable::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ABNORMAL_USE_STACKABLE& req = *reinterpret_cast<MSG_ABNORMAL_USE_STACKABLE*>(&msg);
    if (!packet.get_int(req.m_field10))
    {
        return LineFunc(0x530, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_field14))
    {
        return LineFunc(0x534, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
