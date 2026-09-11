bool get_disjoint_additional_items(
    std::multimap<int, stItemSelectRate>& m, int key,
    std::vector<stItemSelectRate>& out)
{
    // b28d-solo [P222 正向包裹；P13：ORIG 循环初始化区一次性求值 lower/upper_bound
    // （-0x14/-0x18 两具名迭代器），条件仅比较不重复调用]。
    std::multimap<int, stItemSelectRate>::iterator found = m.find(key);
    if (found != m.end())
    {
        for (std::multimap<int, stItemSelectRate>::iterator it = m.lower_bound(key),
                 e = m.upper_bound(key); it != e; ++it)
        {
            out.push_back(it->second);
        }
        return true;
    }
    return false;
}