void CItemLimitEditionMgr::registItem(const stItemLimitEditionItemInfo_t& info)
{
    // ORIG 0x084e975e：new CItemLimitEdition(info)，按 getIPGNO 查重，命中则删旧并 erase，
    // 再 insert（key = 二次调用 getIPGNO —— ORIG 0x84e9803 再次 call getIPGNO，
    // 结果落 -0xc 后作 make_pair 首参；复用首个 key 局部会少一次 call = 判据㉑ 缺调用）。
    // [P212/P309 全头部声明] ORIG 槽序 edition=-0x38（深）/key=-0x34（浅）与 4.4 块内
    // 首次使用序相反 ⇒ 两局部提至函数头按 ORIG 槽深序声明、块内仅赋值。
    unsigned int key;
    CItemLimitEdition* edition;
    edition = new CItemLimitEdition(info);
    key = edition->getIPGNO();
    __gnu_cxx::hash_map<unsigned int, CItemLimitEdition*,
                        __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>,
                        std::allocator<CItemLimitEdition*> >::iterator it
        = m_items.find(key);
    if (it != m_items.end()) {
        // ORIG 0x84e97b0..0x84e97c2：ptEv 后直接 `test %ebx,%ebx; je` 判空 + D1 + _ZdlPv
        // （编译器自带 delete 判空守卫，P141）；显式 if 会多 setne 物化。
        delete it->second;
        m_items.erase(it);
    }
    m_items.insert(std::make_pair(edition->getIPGNO(), edition));
