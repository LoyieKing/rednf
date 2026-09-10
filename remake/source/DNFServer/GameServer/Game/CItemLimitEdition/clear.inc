void CItemLimitEditionMgr::clear()
{
    // ORIG 0x08374d4e：const_iterator it/itEnd（非 const begin()/end() 返回 iterator 后
    // 各经 _Hashtable_const_iterator::C1 转换构造，0x8374d77/0x8374d9e），end 循环外缓存；
    // 循环体 `ptEv; test %ebx,%ebx; je` + D1 + _ZdlPv（编译器判空守卫，P141，无显式 if）。
    typedef __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<CItemLimitEdition*> > ItemMap;
    ItemMap::const_iterator it = m_items.begin();
    ItemMap::const_iterator itEnd = m_items.end();
    for (; it != itEnd; ++it) {
        delete it->second;
    }
    m_items.clear();
