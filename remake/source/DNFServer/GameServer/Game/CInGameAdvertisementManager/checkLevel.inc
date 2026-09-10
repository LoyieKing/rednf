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
