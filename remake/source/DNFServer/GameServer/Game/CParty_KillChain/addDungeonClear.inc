bool CParty::addDungeonClear(const CDungeon* dungeon, int param, int slot)
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x085bde2d 先 call get_unique_id、0x085bde3a 后读记录（cmp %dx,%ax）
        // ⇒ 左操作数写 get_unique_id（求值序对齐；记录在左会把记录读提前到 call 前）。
        if (static_cast<short>(m_member[slot].m_pUser->get_unique_id()) !=
            reinterpret_cast<const KcClearRecView*>(this)
                ->m_recs[i].m_uniqueId)
            continue;

        // P251：ORIG 0x085bde5f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（kind 无槽往返）⇒ 源形 switch(call())。
        switch (getDungeonTypeKind(dungeon))
        {
        case 0:
            // P226 扩展：ORIG 0x085bdebf `mov $1/jmp; mov $0; test; je` 短路
            // 物化岛 ⇒ 语句表达式包整个条件（内不含具名变量）。
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind0) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 10 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 1:
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind12) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 0x1e &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        case 2:
            if (({ (param >= static_cast<int>(
                            reinterpret_cast<const KcDataMgrDailyLimitView*>(
                                G_CDataManager())->m_limitKind12) &&
                        reinterpret_cast<const KcClearRecView*>(this)
                                ->m_recs[i].m_count <= 0x32 &&
                        m_member[slot].m_field14 == 0); }))
            {
                return m_member[slot].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 3);
            }
            continue;
        default:
            return false;
        }
    }
    return false;
