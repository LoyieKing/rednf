// ORIG GetItemInfo(vector&, int) @0x08ac03a6（0xfe 字节）：仅指定 day 组；
// find 未命中时不动 out（ORIG 同）。
void CreateDnfEventSciprt::GetItemInfo(std::vector<std::pair<int, int> >& out, int dayLevel)
{
    std::map<int, std::vector<ItemInfo>* >::iterator it = m_itemMap.find(dayLevel);
    if (it != m_itemMap.end())   // ORIG 结构：ne; je 跳过（命中才处理）
    {
        out.clear();
        std::vector<ItemInfo>* vec = it->second;
        unsigned int i = 0;                // ORIG：i 声明在 n 之前
        unsigned int n = vec->size();      // ORIG：size 只取一次并缓存
        for (; i < n; ++i)
        {
            out.push_back(std::make_pair(vec->at(i).m_itemIndex, vec->at(i).m_count));
        }
    }
}