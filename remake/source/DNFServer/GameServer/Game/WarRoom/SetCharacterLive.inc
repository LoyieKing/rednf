void WarRoom::SetCharacterLive(int seat, CUser* user, bool live, bool a, short b, int c)
{
    // ORIG 086be8c8：装载序 seat→this→live(movzbl)；4.4 sched 恒把 live 转换
    // 提前到 this 前（显式 uchar cast 实测不改形）⇒ N 类装载序，仅 1 行差。
    this->m_liveFlag[seat] = live;
    PacketGuard packet;
    packet.put_header(0, 0x5f);
    packet.put_byte(GetUserSlot(user));
    packet.put_byte(live);
    packet.put_byte(a);
    // ORIG 0x86be94d：外层条件反相——true 路径 je 到函数尾共享 put(0) 块，
    // else 链 fall-through；内层 `m_flag12 != 0` then=put(0) 在前、d4 为 je 目标。
    if (!(((c == 0xff) || (c < 0)) || (c > 5) || (m_slots[c] == 0)))
    {
        if (m_flag12 != 0)
        {
            packet.put_short(0);
        }
        else
        {
            packet.put_short(m_pDungeon->m_warRoomData.m_d4);
        }
    }
    else
    {
        packet.put_short(0);
    }
    packet.put_short(b);
    packet.finalize(true);
    SendToRoom(packet);
}