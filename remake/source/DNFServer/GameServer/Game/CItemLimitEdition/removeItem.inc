void CItemLimitEditionMgr::removeItem(unsigned int itemIdx)
{
    // ORIG 0x084e9876：find 命中则 delete + erase。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(itemIdx);
    if (it != m_items.end()) {
        // [b28c-L29] ORIG 0x084e9876：仅一次 ptEv+test/je（编译器自带 delete
        // 判空守卫，P141）——去掉显式 if(it->second) 双重检查。
        delete it->second;
        m_items.erase(it);
    }
