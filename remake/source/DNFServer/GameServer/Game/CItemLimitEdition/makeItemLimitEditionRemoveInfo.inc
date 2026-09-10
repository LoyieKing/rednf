void CItemLimitEditionMgr::makeItemLimitEditionRemoveInfo(PacketGuard& packet) const
{
    // ORIG 0x08513382：get_index 留计数位，循环遍历 m_items 写 getIPGNO，
    // 末尾 put_byte(index, count)。ORIG 无 buf 局部、无 it->second 判空、end() 缓存。
    int countIdx = packet.get_index();
    packet.put_byte(0);
    int count = 0;
    typedef __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                                __gnu_cxx::hash<unsigned int>,
                                std::equal_to<unsigned int>,
                                std::allocator<CItemLimitEdition*> > ItemMap;
    ItemMap::const_iterator it = m_items.begin();
    ItemMap::const_iterator itEnd = m_items.end();
    for (; it != itEnd; ++it) {
        packet.put_int(it->second->getIPGNO());
        ++count;
    }
    packet.put_byte(countIdx, count);
