int DisPatcher_SetPVPReady::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_SET_READY_STATE& req = *reinterpret_cast<MSG_SET_READY_STATE*>(&msg);
    if (!packet.get_byte(reinterpret_cast<char&>(req.m_0d)))
    {
        return LineFunc(0x1cd3,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
