void CVillageMonsterArea::SendVillageMonsterPosition()
{
    PacketGuard packet;
    packet.clear();
    packet.put_header(0, 0xef);
    int countIdx = packet.get_index();
    int count = 0;
    packet.put_byte(count);
    int deadPad = 0;   // ORIG -0x10 死槽（P232：写后未读）
    (void)deadPad;
    for (std::map<unsigned short, CVillageMonster>::iterator it =
             m_monsters.begin();
         it != m_monsters.end(); ++it)
    {
        CVillageMonster* monster = &it->second;
        if (!monster->MakeVillageMonsterPositionPacket(packet))
            goto SKIP_COUNT;
        ++count;
    SKIP_COUNT:
        ;
    }
    packet.put_byte(countIdx, count);
    packet.finalize(true);
    if (count != 0)
        SendPacket(packet);
