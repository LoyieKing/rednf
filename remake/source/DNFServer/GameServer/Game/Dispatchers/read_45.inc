int Dispatcher_CollectItems::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG：单一 req 基址槽 -0xc，用点 `mov -0xc,%eax; add $0xe/$0x10` 现址。
    MSG_COLLECT_ITEMS& req = reinterpret_cast<MSG_COLLECT_ITEMS&>(msg);
    if (!packet.get_short(req.m_itemIdx))
    {
        return LineFunc(0x5a8f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_count))
    {
        return LineFunc(0x5a90,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
