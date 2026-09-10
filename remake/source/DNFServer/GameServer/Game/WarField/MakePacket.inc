void WarField::MakePacket(int packetId, void* packet)
{
    void* buf = packet;
    switch (packetId)
    {
    case 0x1d:
        ((InterfacePacketBuf*)buf)->put_header(0, 0x1d);
        ((InterfacePacketBuf*)buf)->put_int(rand());
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->put_byte(1);
        ((InterfacePacketBuf*)buf)->put_short(m_mapInfo.m_mapIndex);
        ((InterfacePacketBuf*)buf)->put_byte((int)m_mapInfo.m_monsterMap.size());
        map_monster mob;
        for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
             it != m_mapInfo.m_monsterMap.end(); it++)
        {
            mob = it->second;
            ((InterfacePacketBuf*)buf)->put_short(0);
            ((InterfacePacketBuf*)buf)->put_int(mob.m_mobId);
            ((InterfacePacketBuf*)buf)->put_short((short)mob.m_instId);
            ((InterfacePacketBuf*)buf)->put_int(mob.m_mobIndex);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_level);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_roleType);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_field13);
            ((InterfacePacketBuf*)buf)->put_byte((int)mob.m_field14);
        }
        ((InterfacePacketBuf*)buf)->put_byte(0);
        ((InterfacePacketBuf*)buf)->finalize(true);
        break;
    }
