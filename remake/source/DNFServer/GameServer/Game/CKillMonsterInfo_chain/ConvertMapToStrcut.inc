// ---- ConvertMapToStrcut —— ORIG T 0x0854ccbc（const）----------------------
// empty→1；size>count→0；const_iterator 默认构造后 = begin()（P28 临时中转），
// while (it != end && i < count) 逐项回写 st[i]（i*5 步长、两次独立重算）。
int CKillMonsterInfo::ConvertMapToStrcut(
    const std::map<unsigned int, unsigned char>& inMap,
    stMonsterInfo_t* pInfo, unsigned int count) const
{
    if (inMap.empty())
        return 1;
    if (inMap.size() > count)
        return 0;
    std::map<unsigned int, unsigned char>::const_iterator it;
    it = inMap.begin();
    unsigned int i = 0;
    while (it != inMap.end() && i < count)
    {
        pInfo[i].mobIdx = it->first;
        pInfo[i].diff = it->second;
        it++;
        i++;
    }
    return 1;
}