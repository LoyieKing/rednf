// ORIG T 0x088dcff5：函数局部 static map（guard @0x94f6b78 / map @0x94f6b80）
// 惰性构造；空时调 makeMenuActionHashMap；find(name) 命中返回 second，
// 未命中返回 0x1a。
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
}