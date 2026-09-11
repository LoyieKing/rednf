// ORIG 0x083031d2 ConsistMap：当前图装配怪物/物品链（逐调用与报告一致）。
void CBattle_Field::ConsistMap()
{
    if (m_party == NULL)
        return;

    MapInfo* mapInfo = GetCurrentMapInfo();
    // [W13E 2026-09-11] getMap 现返回 const CMap*（ORIG 类型，prett 串直证）；
    // m_pMap 为非 const 成员 ⇒ const_cast（指针 cast 零指令，机器码不变）。
    m_pMap = const_cast<CMap*>(getMap(mapInfo->m_mapIndex));
                                                 // 实参 = mapInfo->m_mapIndex(+0x04)
    if (m_pMap == NULL)
        return;

    setDungeonClearTrace(mapInfo->m_mapIndex, m_pMap->GetBossCount(), 0, 0);

    CDungeon* dungeon = G_CDataManager()->find_dungeon(m_dungeonIndex);
    if (dungeon == NULL)
    {
        LogManager::logFormat(1, "battle_field.cpp",
                              __PRETTY_FUNCTION__, 0x7f2,
                              "G_CDataManager()->find_dungeon( %d) fail", m_dungeonIndex);
        return;
    }

    int addChampionCnt = getAddChampionMobCnt();
    if (addChampionCnt < 0)
        return;

    std::list<_mapMonster> mobList;
    if (!get_monster_list(mobList))
        return;

    int curX = -1;
    int curY = -1;
    getCurPosXY(curX, curY);
    add_monster_list_bossroom_condition(mobList, curX, curY);

    int monsterCnt = 0;                          // -0x28
    map_monster mob;                             // -0x390（默认构造 081511c8）
    std::vector<int> championList;               // -0x34
    // b28d-solo-BattleDepsB2：ORIG 槽序 seq(-0x38) 浅于 uid(-0x3c) 且 `movl $0,-0x38`
    //   （seq=0）先于 get_rand_int（+0x197/+0x19e）⇒ 声明序 seq 在 uid 前。
    int seq = 0;                                 // -0x38
    int uid = get_rand_int(60000);               // -0x3c
    add_monsters_current_map(mobList, championList, mob, monsterCnt, uid, seq);

    bool killedAll = false;                      // -0x3d
    add_monster_hell_party(mob, killedAll, uid, seq);

    // b28d-solo-BattleDepsB2：ORIG +0x236 `xor $1,%eax; test; je <成功路径>`、失败落
    //   `mov $0,%ebx; jmp <cleanup>` ⇒ 源码为否定守卫早返（P214①/P222）。
    if (!add_monster_blood(mob, uid, seq))
        return;

    {
        m_dungeonClearTracer.SetMaxMonsterCount(monsterCnt);
        add_monster_APC_AI(uid);
        add_monster_APC_AI_by_rate(mob, uid, curX, curY);
        add_monster_additional_champion(championList, addChampionCnt);
        add_monster_event(uid, curX, curY);
        set_monster_ability();
        insert_tournamentRoundMonster();

        int itemUid = get_rand_int(60000);       // -0x44
        add_item_curr_map(itemUid, uid);
        mapInfo->m_dropItemCnt = itemUid;        // +0x40

        // ORIG 083034ed..0830351c：m_pMap->m_ec(+0xec)==0 时取
        // *(uint*)(G_CDataManager()+0x49d0) 作 SelectDonsterItemDropLimit 上限。
        // 即 CDataManager::m_serverParameter.m_4968（CDataManager+0x68+0x4968）。
        // b28d-solo-BattleDepsB2：ORIG 无 dm/dropLimit 具名槽（test;jne; call; mov 0x49d0;
        //   jmp join; else mov 0xec; join: mov %eax,0x4(%esp)）⇒ 三目右值直传；
        //   具名局部版多 2 槽 4 条并把全帧推深 8（P0/P183）。
        mapInfo->SelectDonsterItemDropLimit(
            m_pMap->m_ec == 0
                ? static_cast<unsigned int>(
                      G_CDataManager()->m_serverParameter.m_4968)
                : static_cast<unsigned int>(m_pMap->m_ec));
    }
}