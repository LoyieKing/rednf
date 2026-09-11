void CDeathTower::CStage::makeStagePacket(PacketGuard& packet)
{
    // [b28d-solo-CDeathTower] verdict=AE（orig 221 / our 221，b28d_cmp ndiff=0）：P167 CLEAR（40/40）。
    // 修法 = ①P226 裸语句表达式 `({ getCurrentStage(); })` 复现 ORIG 0x0846184x
    // `mov %eax,%edx; mov 0xc(%ebp),%eax` 实参先中转 edx 形（裸实参恒产 this 先 3 条）；
    // ②第一循环增量 `++it` 移入体尾（ORIG 0x08461982 ++it 先于 0x084619ad ~map_monster，
    // 4.1 EH 区域划分）；③P200 双向 cast（mob.m_instId 加 `(int)(short)`、m_itemIndex 去 cast）。
    packet.put_short(({ getCurrentStage(); }));
    packet.put_int(get_rand_int(0));
    packet.put_short(m_mapInfo.m_mapIndex);
    packet.put_byte(m_mapInfo.m_monsterMap.size());

    // [b28d-solo-CDeathTower] ORIG 0x08461982 `++it` 先于 0x084619ad
    // `~map_monster`（体尾手动增量，4.1 的 EH 区域划分）⇒ 增量移入循环体末尾。
    for (std::map<int, map_monster>::iterator it = m_mapInfo.m_monsterMap.begin();
         it != m_mapInfo.m_monsterMap.end(); )
    {
        map_monster mob(it->second);   // ORIG 栈拷贝（C1ERKS）后逐字段写包
        packet.put_int(mob.m_mobId);
        packet.put_short((int)(short)mob.m_instId);
        packet.put_int(mob.m_mobIndex);
        packet.put_byte(mob.m_level);
        packet.put_byte(mob.m_roleType);
        packet.put_byte(mob.m_field13);
        packet.put_byte(mob.m_field14);
        ++it;
    }
    // ORIG 0x84619ed 起：m_mapInfo.m_itemMap（+0x30）遍历。序列化格式与
    // CBossStage::SendMapInfoPacket 不同（此处 itemIndex 仅 short、m_addInfo
    // 为 int、endurance word），按 ORIG 逐字段转写。
    packet.put_byte((int)m_mapInfo.m_itemMap.size());
    for (std::map<int, map_item>::iterator it = m_mapInfo.m_itemMap.begin();
         it != m_mapInfo.m_itemMap.end(); ++it)
    {
        map_item item = it->second;        // ORIG 21 dword 栈拷贝
        packet.put_byte((int)(signed char)item.m_standardLevel);   // +0x01
        packet.put_short(item.m_itemIndex);                       // +0x04 低 2 字节
        packet.put_int(item.m_item.m_addInfo);                     // m_item+0x02
        packet.put_int(item.m_item.get_add_info());                // m_item+0x07
        packet.put_short((int)item.m_item.m_fieldb);               // m_item+0x0b（endurance）
        packet.put_byte((int)item.m_item.m_amp.getAbilityType());  // m_item+0x11
        packet.put_short((int)item.m_item.m_amp.getAbilityValue());
    }
}