int CItemList::GetRestrictCode(int itemIdx)
{
    __gnu_cxx::hash_map<unsigned int, int, __gnu_cxx::hash<unsigned int>,
                        std::equal_to<unsigned int>,
                        std::allocator<int> >::iterator it =
        m_lottoMap.find((unsigned int)itemIdx);
    if (it == m_lottoMap.end())
        return 0;
    return it->second;
