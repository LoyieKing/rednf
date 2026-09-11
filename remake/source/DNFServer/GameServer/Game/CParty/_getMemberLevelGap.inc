int CParty::_getMemberLevelGap()
{
    if (m_memberLevelGap == -1)
    {
        // ORIG 槽布局（声明序决定）：min -0x18, max -0x1c, levelMin -0x14,
        // levelMax -0x10, i -0xc；初始化语句序 min 先 max 后（asm $0x63 在前）。
        int levelMax;   // -0x10
        int levelMin;   // -0x14
        int minLevel;   // -0x18
        int maxLevel;   // -0x1c
        int i;
        minLevel = 99;
        maxLevel = 0;
        for (i = 0; i <= 3; ++i)
        {
            if (_checkValidUser(i) != 1)
                continue;
            levelMin = ((CUserCharacInfo*)m_member[i].m_pUser)->get_charac_level();
            minLevel = std::min(minLevel, levelMin);
            levelMax = ((CUserCharacInfo*)m_member[i].m_pUser)->get_charac_level();
            maxLevel = std::max(maxLevel, levelMax);
        }
        m_memberLevelGap = (maxLevel - minLevel) < 0 ? -(maxLevel - minLevel)
                                                     : (maxLevel - minLevel);
    }
    return m_memberLevelGap;
}