int CParty::checkInoutConditionDungeon()
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS CParty::get_user + memset（2 项）。@0x85ac59c
    if (m_battleField.m_pDungeon == 0)
        return 10;
    if (GetEPLPState() <= (char)1)
        return 10;
    for (int i = 0; i < 4; ++i)
    {
        // P0/P183：ORIG 0x085ac59c 段三次独立 get_user（条件 1 次 + 疲劳比较 2 次），
        // 无 u 缓存（p167 曾 MISS 2）。
        if (get_user(i) == 0 ||
            (char)m_battleField.m_pDungeon->get_blood_dungeon_type() > 0)
            continue;
        if (get_user(i)->getCurCharacTotalFatigue() ==
            get_user(i)->getCurCharacTotalMaxFatigue())
            return 1;
    }
    unsigned char flag = 0;
    Inven_Item items[4];
    int param[4];
    // ORIG 0x085ac59c：两次 call memset（0xf4 / 0x10）；`={0}` 会被 4.4 内联成 movl。
    memset(items, 0, 0xf4);
    memset(param, 0, 0x10);
    if (CheckEnterAncientDungeon(m_battleField.m_pDungeon, flag, items, param) == 1)
    {
        int r = CheckEnterDimensionDungeon(m_battleField.m_pDungeon);
        if (r == 0xab || r == 0xad)
            return 4;
        r = check_dungeon_start(m_battleField.m_pDungeon);
        if (!(r != 10 && (r < 10 || 4 < r - 0xf3u)))
            return 7;
        if (CheckEnterVillageAttackRevenge(m_battleField.m_pDungeon) != 0)
            return 7;
        CWorldMap* worldMap = 0;
        int worldIdx = G_GameWorld()->GetWorldMapIndex(m_manager);
        if (worldIdx > 0)
        {
            worldMap = G_CDataManager()->m_worldMapList.find_world_map(worldIdx);
        }
        CBattle_Field& bfView = *reinterpret_cast<CBattle_Field*>(&m_battleField);
        bool hellDun = false;
        if ((worldMap != 0 &&
             worldMap->IsInHellDungeon() != 0 &&
             bfView.IsEnableHellDungeon() != 0) &&
            bfView.m_hellPartyMode == 1)   // ORIG this+0xcd4（BF+0x1b0 m_hellPartyMode）
        {
            hellDun = true;
        }
        if (hellDun)
        {
            std::vector<int> vec;
            int sealR = CheckDestroyConditionSealDoor(
                m_battleField.m_pDungeon, vec, worldMap->m_field34);
            if (sealR == 1)
                m_sealDoorFlag = 1;
            else
                m_sealDoorFlag = 0;
            if (sealR != 1)
                return 8;
        }
        bool noFat = true;
        if (m_battleField.m_bloodState == 1 ||
            (worldMap != 0 && worldMap->hasDeathTower() == 1) ||
            m_battleField.m_pDungeon->isNoFatigueDungeon() == 1 ||
            m_battleField.m_pDungeon->m_advanceAltarSurvivalFlag == 1)
        {
            noFat = false;
        }
        if (noFat && CheckMemberFatigue() > 0)
            return 1;
        return 9;
    }
    return 2;
