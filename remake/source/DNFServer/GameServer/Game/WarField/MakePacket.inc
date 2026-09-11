// [b28d-solo-WarField] AE（158=158）：P286 单 case switch 复现「形参载寄存器比较」
// （if 形 4.4 恒折 cmpl mem）+ P68 cast-this 形（void* buf + ((InterfacePacketBuf*)buf)->）
// 复现 4.1 每个成员调用「先载 this 再压常量」的实参装载序。@0x086b92e2
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
}