// ---------------------------------------------------------------------------
// getDungeonDropTestFlag —— ORIG T 0x085bdc76
// 报告：docs/class_func_reports/CParty/getDungeonDropTestFlag.md
// 任一 GM 成员在存图模式 → true。
// ---------------------------------------------------------------------------
bool CParty::getDungeonDropTestFlag()
{
    for (int i = 0; i < 4; ++i)
    {
        CUser* user = get_user(i);                       // thunk 0x08145764
        if (user == 0)
            continue;
        if (user->isGMUser() && getDungeonMapSaving())
            return true;
    }
    return false;
}