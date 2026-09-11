int Dispatcher_GrowthWeaponEventUseMaterial::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x T：读 int(msg+0xd)，失败记 LineFunc(0x646a)
    MSG_GROWTH_EQUIPMENT& p = reinterpret_cast<MSG_GROWTH_EQUIPMENT&>(msg);
    if (!packet.get_int(p.m_0d))
    {
        return LineFunc(0x646a, __PRETTY_FUNCTION__, 0, 0);
    }
    // ORIG 0x T：读 int(msg+0x11)，失败记 LineFunc(0x646d)
    if (!packet.get_int(p.m_11))
    {
        return LineFunc(0x646d, __PRETTY_FUNCTION__, 0, 0);
    }
    return 0;
}