// ORIG T 0x085b603e（getMemberMinLevel.md）：std::min 形态最小等级；
// 无有效成员时保持初值 INT_MAX。
int CParty::getMemberMinLevel()
{
    int level;                       // P112 槽序：ORIG level@-0x10 浅于 minLevel@-0x14
    int minLevel = 0x7fffffff;
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        level = m_member[i].m_pUser->get_charac_level();
        minLevel = std::min(minLevel, level);
    }
    return minLevel;
}