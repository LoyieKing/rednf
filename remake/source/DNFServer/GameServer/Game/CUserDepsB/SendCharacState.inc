// ORIG T 0x0814701e sz 0x146（静态）。包头 0x22a：byte 数量 + 每档 (int level,
// byte state) + byte state 参数，finalize(true) 后 Send。
void LevelupSupportEventManger::SendCharacState(CUser* user, int state)
{
    // b28d-solo-CUserDepsB: AE——P13/P212：ORIG 0x08147076 先把 i 清 0（-0x10）、
    // size() 一次求值缓存到 -0xc（循环头不重调），故写「i=0 声明 + count=size()
    // 声明 + for(;i<count;)」；帧 0x30/槽位与 ORIG 全等，93=93 逐条同构。
    // ORIG 0x0814701e。
    PacketGuard packet;                            // C1 0x0858dd4c
    packet.clear();
    packet.put_header(0, 0x22a);
    packet.put_byte(static_cast<int>(user->m_levelupEvents.size()));
    unsigned int i = 0;
    unsigned int count = user->m_levelupEvents.size();
    for (; i < count; ++i)
    {
        packet.put_int(static_cast<int>(user->m_levelupEvents[i].m_level));
        packet.put_byte(static_cast<int>(user->m_levelupEvents[i].m_state));
    }
    packet.put_byte(state);
    packet.finalize(true);
    user->Send(packet);                            // T 0x086485ba
}