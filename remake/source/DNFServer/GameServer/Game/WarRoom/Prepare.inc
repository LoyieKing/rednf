void WarRoom::Prepare()
{
    int dungeonIdx =
        G_CDataManager()->m_warAreaCounter.m_warAreaDungeon[(unsigned char)m_levelBand];
    CDungeon* dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (dungeon == 0)
    {
        LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x66e,
                              "WARROOM G_CDataManager()->find_dungeon( %d) fail", 0);
    }
    else
    {
        m_pDungeon = dungeon;
        // ORIG 0x86bd65e：int 直落 callee-saved %ebx，免 scratch 两行。
        register int mapIdx = m_pDungeon->m_warRoomData.m_10;
        // ORIG 0x86bd6d0：int 变量直存使 store 块 this→eax 先行、值→edx 后置。
        int map = (int)G_CDataManager()->find_map(mapIdx);
        if (map == 0)
        {
            LogManager::logFormat(1, "WarRoom.cpp", __PRETTY_FUNCTION__, 0x678,
                                  "WARROOM Find Map[%d] Error",
                                  m_pDungeon->m_warRoomData.m_10);
        }
        else
        {
            m_fp = map;
        }
    }
}