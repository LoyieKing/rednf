int DisPatcher_WalkoutParty::read(PacketBuf& packet, MSG_BASE& msg)

{
    MSG_WALKOUT_PARTY& m = *reinterpret_cast<MSG_WALKOUT_PARTY*>(&msg);
    if (!packet.get_byte(m.m_walkoutSlot))                // +0x0d
    {
        return LineFunc(0x202b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
