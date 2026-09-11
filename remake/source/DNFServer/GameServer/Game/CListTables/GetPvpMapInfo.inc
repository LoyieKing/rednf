// [b28d-solo-CListTables AE] ORIG 0x0834f5c8：`info = it->second;` 结构赋值即复现
// ORIG 的单次 ptEv + 寄存器内比较直跳 + memcpy(5)（4.4 对 5 字节 POD 赋值同形）；
// 旧 `if (&info != &it->second) memcpy(&info,&it->second,5)` 两次 ptEv + setne
// 物化（P197④ 镜像）⇒ 38=38 AE。
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
}