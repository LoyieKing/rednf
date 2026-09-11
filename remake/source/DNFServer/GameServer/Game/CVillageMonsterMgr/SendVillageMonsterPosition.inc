// ============================================================================
// CVillageMonsterArea::SendVillageMonsterPosition —— ORIG T 0x86b3962
//   0xef 位置包：put_header(0,0xef)；预留 count 字节；遍历怪物调用
//   MakeVillageMonsterPositionPacket（成功则 count++）；回填 count；
//   finalize(true)；count!=0 时 SendPacket。
// ============================================================================
// [b28d-solo-CVillageMonsterMgr] N 封顶（ORIG 0x086b3962，N82/N112 族）：
// float framePad 已对齐帧 0x40 与深槽（begin -0x2c/countIdx -0x28/packet -0x24），
// 残余 = end/count 槽浅 4（-0x14/-0x10 vs ORIG -0x18/-0x14）+ ORIG goto 岛 nop×1；
// 单 double pad 深 8、双 double 帧 0x50 均反劣。p167 CLEAR（17/17）。
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
}