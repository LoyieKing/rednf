bool AradBonusItem::find(unsigned int itemId, Data& out)
{
    // ORIG 0x81a252c：map<int,Data> 命中写 out（拷 10 字节）返回 true。
    int key = itemId;   // ORIG 具名 key 槽（-0x1c）
    std::map<int, Data>::iterator it = m_bonusMap.find(key);
    if (it != m_bonusMap.end())
    {
        out = Data(it->second);
        return true;
    }
    return false;
