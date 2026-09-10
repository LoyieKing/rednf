void Dispatcher_GoldTakeIncreasingAmount::send(CUser* user, ParamBase& param)
{
    // ORIG 0x081d56ac：header(1,0x174)+byte p8
    stGoldTakeIncreasingAmount& p = static_cast<stGoldTakeIncreasingAmount&>(param);
    PacketGuard pkt;
    pkt.put_header(1, 0x174);
    pkt.put_byte(p.m_8);
    pkt.finalize(true);
    user->Send(pkt);
