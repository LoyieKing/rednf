int CParty::getMemberMaxLevel()
{
    int level;                                          // -0x10（槽位对齐 ORIG）
    int maxLevel = 0;                                   // -0x14
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    return maxLevel;
}
