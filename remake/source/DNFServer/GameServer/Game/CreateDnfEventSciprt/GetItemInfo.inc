void CreateDnfEventSciprt::GetItemInfo(std::vector<std::pair<int, int> >& out)
{
    std::map<int, std::vector<ItemInfo>* >::iterator tmp = m_itemMap.begin();  // ORIG 冗余 begin（结果未用）
    std::map<int, std::vector<ItemInfo>* >::iterator itEnd = m_itemMap.end();
    out.clear();
    for (std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.begin(); it != itEnd; it++)  // ORIG：后缀 ++
    {
        unsigned int i = 0;                // ORIG：i 在外层循环体入口初始化
        unsigned int n = it->second->size();   // ORIG：外层不存 vec，链式取 size
        for (; i < n; ++i)
        {
            std::vector<ItemInfo>* vec = it->second;   // ORIG：vec 在内层才存入槽
            out.push_back(std::make_pair(vec->at(i).m_itemIndex, vec->at(i).m_count));
        }
    }
