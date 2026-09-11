int Dispatcher_GrowthWeaponEventChangeInfinity::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x63fc)
    MSG_GROWTH_EQUIPMENT& p = reinterpret_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    if (!packet.get_int(p.m_0d))
    {
        return LineFunc(0x63fc, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 int(msg+0x11)，失败记 LineFunc(0x63ff)
    if (!packet.get_int(p.m_11))
    {
        return LineFunc(0x63ff, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}