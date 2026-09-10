int DisPatcher_FinishLoad::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_FINISH_LOADING& req = *reinterpret_cast<MSG_FINISH_LOADING*>(&msg);
    if (!packet.get_int(req.m_0d))
    {
        return LineFunc(0x5f7,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(req.m_11))
    {
        return LineFunc(0x5f8,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
