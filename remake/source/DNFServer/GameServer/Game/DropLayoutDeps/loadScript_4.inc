int CObjectDrop::loadScript(char const* path)
{
    // ORIG T 0x08538352：先把队伍加成区 (+0xa8..0xf7) 全写 1.0f，再非虚直呼
    // CMonsterDrop::loadScript（0x08537938）。
    for (int i = 0; i <= 4; ++i)
        for (int j = 0; j <= 3; ++j)
            m_partyBonus[i][j] = 1.0f;
    return CMonsterDrop::loadScript(path);
