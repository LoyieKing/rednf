int Dispatcher_ObjectBringUp::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_OBJECT_BRING_UP& req = *reinterpret_cast<MSG_OBJECT_BRING_UP*>(&msg);
    if (!packet.get_short(req.m_field0e))
    {
        return LineFunc(
            0x56aa,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    return 0;
}
