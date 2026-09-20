int Dispatcher_EnterDisjointStore::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_ENTER_DISJOINT_STORE& req = *reinterpret_cast<MSG_ENTER_DISJOINT_STORE*>(&msg);
    if (!packet.get_short(reinterpret_cast<short&>(req.m_field0e)))
    {
        return LineFunc(0x2aa8, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}
