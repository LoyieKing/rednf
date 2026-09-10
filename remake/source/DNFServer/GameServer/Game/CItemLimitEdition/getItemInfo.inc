CItemLimitEdition* CItemLimitEditionMgr::getItemInfo(unsigned int itemIdx) const
{
    // ORIG 0x0817a0a8：find；未命中返回 0，命中返回 second。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::const_iterator it
        = m_items.find(itemIdx);
    if (it == m_items.end())
        return 0;
    return it->second;
