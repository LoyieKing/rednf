void makeMenuActionHashMap(std::map<std::string, int>& menuMap)
{
    if (!menuMap.empty())
        menuMap.clear();
    for (int i = 0; i <= 0x19; ++i)
        menuMap.insert(std::make_pair(g_MainHudMenuActionString[i], i));
}