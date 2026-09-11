// ORIG clearScript @0x08ac032a（0x7b 字节）。逐指令转写【ORIG 原样，含缺陷】：
// 循环体内无迭代器自增，且把 mapped value 首 4 字节当堆指针做显式析构 + delete。
void CreateDnfEventSciprt::clearScript()
{
    std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.begin();
    std::map<int, std::vector<ItemInfo>* >::iterator itEnd = m_itemMap.end();  // ORIG：begin 后即缓存
    for (; it != itEnd /* 无 ++it，ORIG 原样 */; )
    {
        register std::vector<ItemInfo>* victim = it->second;   // ORIG：victim 驻留 ebx
        if (victim != NULL)                                     // 08ac036b
        {
            victim->~ItemInfoVector();                          // 显式 ~vector（08ac06fa）
            ::operator delete(victim);                          // _ZdlPv（087244f0）
        }
    }
    m_itemMap.clear();                                          // 08ac08e2
}