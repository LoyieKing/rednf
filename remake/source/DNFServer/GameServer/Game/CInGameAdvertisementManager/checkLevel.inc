// ORIG T _ZN27CInGameAdvertisementManager10checkLevelEP5CUserPc @0x080fca9e
//（报告 checkLevel.md）：data 逗号分隔等级表 → ExplodeString → atoi 逐项；
// 命中 -1（表尾）或 user 当前等级即返 1。
bool CInGameAdvertisementManager::checkLevel(CUser* user, char* data)
{
    if (user == 0)
    {
        return false;
    }
    char* list[0x79];
    int count = DNFFLib::ExplodeString(data, ",", list, 0x78);
    for (int i = 0; i < count; ++i)
    {
        int lv = atoi(list[i]);
        if (lv == -1)
        {
            return true;
        }
        if (user->get_charac_level() == lv)
        {
            return true;
        }
    }
    return false;
}