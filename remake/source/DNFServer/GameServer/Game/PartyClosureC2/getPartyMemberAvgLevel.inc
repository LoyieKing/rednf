int CParty::getPartyMemberAvgLevel()
{
    // 报告：docs/class_func_reports/CParty/getPartyMemberAvgLevel.md（T 0x0859f582）：
    // 有效成员等级和/人数；无人 → 0（-0x14 槽为 ORIG 保留死变量，保持帧布局一致）。
    int sum = 0;
    int level = 0;
    int validCount = 0;
    for (int i = 0; i <= 3; ++i)
    {
        if (!_checkValidUser(i))
            continue;
        sum += m_member[i].m_pUser->get_charac_level();
        ++validCount;
    }
    if (validCount == 0)
        return 0;
    return sum / validCount;
