int CVillageMonsterArea::OnCreateVillageMonster()
{
    if ((unsigned int)m_field34 <= (unsigned int)m_monsters.size())
        return 0;
    STAttackedMonster* attacked = GetAttackedMonster(get_rand_int(0x2710));
    if (attacked == 0)
        return 0;
    CVillageMonster monster(attacked->m_id,
                            attacked->m_field04, attacked->m_field08,
                            (int)(m_monsters.size() % 3), this,
                            attacked->m_field14, attacked->m_field18);
    unsigned short uid = monster.GetUID();
    m_monsters.insert(std::make_pair((unsigned short)uid, monster));
    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0xf6);
        packet.put_short(monster.GetUID());
        packet.put_short(attacked->m_id);
        packet.put_byte(0);
        packet.put_short(attacked->m_field04);
        packet.put_short(attacked->m_field08);
        packet.finalize(true);
        SendPacket(packet);
    }
    G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x86,
                                m_field38, m_field28, m_field2c);
    return 1;
