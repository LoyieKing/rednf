void CParty::DungeonPermission(int param)
{
    // P52：ORIG 0x085b1300 先把 m_dungeonIndex 物化 %ebx 跨 G_CDataManager call。
    CDungeon* dungeon = G_CDataManager()->find_dungeon(
        m_battleField.m_dungeonIndex);
    if (dungeon == 0)
        return;

    const int minLevel = dungeon->get_min_level();
    for (int i = 0; i <= 3; ++i)
    {
        bool granted = false;
        if (!_checkValidUser(i))
            continue;
        // P0：ORIG 0x085b1371 无 user 具名槽（直接 this 载入 → call get_charac_level）。
        if (minLevel > static_cast<int>(
                           m_member[i].m_pUser->get_charac_level()))
            continue;

        // P251：ORIG 0x085b139f `cmp $1,%eax; je; cmp $2,%eax; je;
        // test %eax,%eax; jne`（diff 无槽往返）⇒ 源形 switch(call())。
        switch (kcbf(this).get_dungeon_diff())
        {
        case 0:
            granted = m_member[i].m_pUser->AddDungeonClear(
                m_battleField.m_dungeonIndex, 1);
            break;
        case 1:
            // P226：ORIG 0x085b1403 `cmp; setle %al; test %al,%al; je 岛` 物化。
            if (({ (static_cast<int>(
                        reinterpret_cast<const KcDataMgrDailyLimitView*>(
                            G_CDataManager())->m_limitDiff1) <= param); }))
                granted = m_member[i].m_pUser->AddDungeonClear(
                    m_battleField.m_dungeonIndex, 2);
            break;
        case 2:
            // P167/P183：ORIG 0x085b147e/0x085b1486 两次完整重读
            // getUltimateDungeonList（无指针缓存），0x085b14aa 用 operator==
            // 比较后 jne 跳过授予；ORIG 0x085b1476 调 const 版 set::find。
            if (static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())
                    ->find(dungeon->get_index()) ==
                static_cast<const std::set<int>*>(
                    G_CDataManager()->getUltimateDungeonList())->end())
                goto DUNGEON_PERM_NEXT;
            granted = addDungeonClear(dungeon, param, i);
        DUNGEON_PERM_NEXT:
            break;
        default:
            break;
        }

        if (granted)
        {
            PacketGuard packet;
            packet.put_header(0, 5);
            packet.put_short(1);
            packet.put_short(m_battleField.m_dungeonIndex);
            packet.put_short(kcbf(this).get_dungeon_diff() + 1);
            packet.finalize(true);
            m_member[i].m_pUser->Send(packet);
        }
    }
