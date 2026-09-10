bool CBossStage::RandomConsistMap(CUser& user)
{
    int mapIdx = 0;
    if (randomMapIndex(mapIdx) != true)
        return false;


    // ORIG 0x0814f462/46f 两条件各自 test/je 直落 L66，无 choose 岛：嵌套 if。
    if (user.isGMUser())
        if (user.m_gmBossMapIndex != 0)
    {
        mapIdx = user.m_gmBossMapIndex;
        user.m_gmBossMapIndex = 0;
    }

    CMap* map = static_cast<CMap*>(G_CDataManager()->find_map(mapIdx));
    // ORIG 0x0814f4a3..b3 choose 岛无 bool 槽（mov$1/jmp/mov$0 + 统一 test）。
    if (map == 0 || !consistMap(map))
        return false;
    // ORIG this+0x9e8 = m_startTick
    m_startTick = (int)GlobalData::s_systemTime_.getCurTickCount();
    return true;
