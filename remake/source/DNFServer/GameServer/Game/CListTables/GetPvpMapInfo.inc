bool CMapList::GetPvpMapInfo(int idx, PvPMapInfo& info)
{
    // [b28c-21 代落 C28L35 spec] 初始化式直构：ORIG find 直接落 -0x10，无 C1Ev 默认构造。
    std::map<int, PvPMapInfo>::iterator it = mapPvpMapInfo_.find(idx);
    if (it != mapPvpMapInfo_.end())
    {
        // [b28d-solo-CListTables] 结构赋值即复现 ORIG 单次 ptEv + 直跳比较 + memcpy(5)。
        info = it->second;
        return 1;
    }
    return 0;
