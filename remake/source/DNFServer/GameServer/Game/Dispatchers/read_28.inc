int Dispatcher_MotionHack::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_MOTION_HACK& req = *reinterpret_cast<MSG_MOTION_HACK*>(&msg);
    if (!packet.get_byte(req.m_field0d))
    {
        return LineFunc(0x414, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_field0e))
    {
        return LineFunc(0x416, __PRETTY_FUNCTION__, 0, 0);
    }
    if (!packet.get_int(req.m_field12))
    {
        return LineFunc(0x418, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
