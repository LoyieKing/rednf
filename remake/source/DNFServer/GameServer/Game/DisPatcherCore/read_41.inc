int DisPatcher_UseCoin::read(PacketBuf& packet, MSG_BASE& msg)
{
    // 具名字段（MSG_USE_COIN pack(1) m_0d ushort；ORIG 0x081ca5f3 add $0xd +
    //  call _ZN9PacketBuf9get_shortERt → 2B 无符号，重载一致）
    MSG_USE_COIN& req = *reinterpret_cast<MSG_USE_COIN*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0x1ebb,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
