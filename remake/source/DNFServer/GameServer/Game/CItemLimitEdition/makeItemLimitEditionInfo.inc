void CItemLimitEditionMgr::makeItemLimitEditionInfo(PacketGuard& packet) const
{
    // ORIG 0x085132ca：get_index 留计数位，循环遍历 m_items 调 makeInfoPacket，
    // 末尾 put_byte(index, count)。ORIG 无 buf 局部（每次 mov 0xc(%ebp),%eax 重读形参）、
    // 无 it->second 判空（0x8513324 段 ptEv 后直接取 second）、end() 循环外缓存一次。
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
        it->second->makeInfoPacket(packet);
        ++count;
    }
    packet.put_byte(countIdx, count);
