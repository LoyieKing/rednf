int getMenuAction(const char* name)
{
    static std::map<std::string, int> menuHashMap;
    if (menuHashMap.empty())
        makeMenuActionHashMap(menuHashMap);
    std::map<std::string, int>::const_iterator it =
        menuHashMap.find(std::string(name));
    if (it != menuHashMap.end())
        return it->second;
    return 0x1a;
