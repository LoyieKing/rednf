int WarRoom::HandleWpPerMonster(CUser* user, int a, int b, CUser* killer)
{
    map_monster monster;
    int count = GetWaiterCount();
    if (count == 0)
        return 3;
    int wp = 0;
    for (int i = 0; i <= 5; ++i)
    {
        {
            // ORIG 086c05ab：slot-null je→086c065c(nop;fall 086c065d) 空岛；
            // isTarget-false je 直落 end；body 尾显式 jmp end(086c065a)。
            if (m_slots[i] == 0)
                goto WP_SLOT;
            bool isTarget = (killer != 0) && (m_slotsUser[i] == killer);
            if (isTarget)
            {
                wp = m_field.HandleWpPerMonster(killer, a, b, monster);
                killer->WarAreaKill(monster.m_roleType, wp, monster.m_field18);
                goto WP_JOIN;
            }
        }
WP_JOIN:
        ;
WP_SLOT:
        ;
    }
    // ORIG 0x86c0670/0x86c06c3：movw $0xffff 初始化 + movzwl 回读 ⇒ unsigned short。
    unsigned short killerUid = 0xffff;
    if (killer != 0)
        killerUid = killer->get_unique_id();
    PacketGuard packet;
    packet.put_header(0, 0x151);
    packet.put_int(a);
    packet.put_short(killerUid);
    packet.put_int(b);
    packet.put_short(wp);
    packet.finalize(true);
    SendToRoom(packet);
    return 0;
