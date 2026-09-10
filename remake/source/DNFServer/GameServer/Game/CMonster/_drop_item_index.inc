int _drop_item_index(int randomValue,
                     const std::vector<std::pair<int, int> >& entries)
{
        // ORIG 0x08349672：先清零 accumulated，再构造 entry；循环内经 make_pair
    // 临时对走 pair::operator=(pair&&)（O0 生成 move op= 调用，非内联拷贝）。
        int accumulated = 0;
    std::pair<int, int> entry;
    for (std::vector<std::pair<int, int> >::const_iterator it = entries.begin();
         it != entries.end(); it++)
    {
                entry = std::pair<int, int>(*it);
        accumulated += entry.second;
        if (randomValue < accumulated)
            return entry.first;
    }
    return -2;
