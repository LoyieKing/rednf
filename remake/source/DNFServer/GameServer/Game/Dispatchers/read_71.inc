int Dispatcher_HatchCreature::read(PacketBuf& packet, MSG_BASE& msg)
{
    MSG_HATCH_CREATURE& req = *reinterpret_cast<MSG_HATCH_CREATURE*>(&msg);
    if (!packet.get_byte(req.m_itemSpace))
    {
        return LineFunc(0x2518,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_slotIdx))
    {
        return LineFunc(0x251b,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}
