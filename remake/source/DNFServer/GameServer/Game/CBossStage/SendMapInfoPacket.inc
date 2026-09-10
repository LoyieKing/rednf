int CBossStage::SendMapInfoPacket()
{
    PacketGuard packet;
    packet.put_header(0, 0x1e2);
    packet.put_short(m_stageCur);                          // 当前层
    packet.put_int(m_stageData.m_mapBasisLevel);           // seed 值（ORIG +0xa3c）
    packet.put_short(m_stageData.m_mapIndex);              // 当前图索引

    // P0：ORIG 0x0814ce5e `mov 0x8(%ebp),%eax; add $0xa00,%eax` ⇒ m_stageData
    //   直访问（无 MapInfo& 引用槽）。
    packet.put_byte((int)m_stageData.m_monsterMap.size());
    // [b28d-62] P96：ORIG 用非 const `_Rb_tree_iterator`（p167 MISSING ptEv/*++/!= 族）
    //   ⇒ 改 iterator，消 const_iterator 转换 ctor ×4 与 const 版 operator 族。
    // [b28d-solo] ORIG 0x0814cf8f..cf9a 的 `++it` 在循环体尾（回边前）、
    //   D1/end/ne 在条件块（0x0814cfb7..cfed）⇒ 源形 while + 体尾 ++it。
    std::map<int, map_monster>::iterator it = m_stageData.m_monsterMap.begin();
    while (it != m_stageData.m_monsterMap.end())
    {
        map_monster mob(it->second);               // ORIG 栈拷贝后逐字段写包
        packet.put_int(mob.m_mobId);               // +0x00
        packet.put_short((int)(short)mob.m_instId);       // +0x04
        packet.put_short(mob.m_mobIndex);                 // +0x0c（ORIG 无 cwtl）
        packet.put_byte((int)(unsigned char)mob.m_level); // +0x10
        packet.put_byte((int)(signed char)mob.m_roleType);// +0x08
        packet.put_byte((int)(unsigned char)mob.m_field13);
        packet.put_byte((int)(unsigned char)mob.m_field14);
        ++it;
    }

    packet.put_byte((int)m_stageData.m_itemMap.size());
    for (std::map<int, map_item>::iterator it = m_stageData.m_itemMap.begin();
         it != m_stageData.m_itemMap.end(); ++it)
    {
        // ORIG 0x0814d044..058 `rep movsl` 0x15 dword ⇒ 栈上整体拷贝非引用
        map_item item(it->second);
        // +0x01 等级字节（ORIG movzbl+movsbl 后 put_byte：0814d05a）
        packet.put_byte((int)(signed char)(unsigned char)item.m_standardLevel);
        // [b28f-SweepCBossStage] 真 bug：ORIG 0x0814d083 `mov -0x84(%ebp),%eax` →
        //   call 080d9ea4（put_short），非 put_int（080cb93c）。原写 put_int 令
        //   0x1e2 包 itemIndex 后所有字段错位 2 字节（p167: put_short −1 / put_int +1）。
        packet.put_short(item.m_itemIndex);                // +0x04
        packet.put_int(item.m_item.m_addInfo);             // ORIG 0x0814d088 32 位读 -0x76
        packet.put_int(item.m_item.get_add_info());        // Inven_Item+7
        packet.put_short((int)item.m_item.m_fieldb);       // ORIG 0x0814d0ba -0x6d（Inven_Item+0xb）
        packet.put_byte((int)item.m_item.m_amp.getAbilityType() & 0xff);
        packet.put_short((int)item.m_item.m_amp.getAbilityValue() & 0xffff);
    }
    packet.finalize(true);
    m_party->send_to_party(packet);
    return 1;
