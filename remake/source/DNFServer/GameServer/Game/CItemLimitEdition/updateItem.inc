bool CItemLimitEditionMgr::updateItem(unsigned int itemIdx, unsigned int count)   // [b28d-Main·P292] int→bool
{
    // ORIG 0x084e9904：find 未命中返回 0；命中 updateCurSellNum 返回 1。
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(itemIdx);
    if (it == m_items.end())
        return false;
    it->second->updateCurSellNum(count);
    return true;
