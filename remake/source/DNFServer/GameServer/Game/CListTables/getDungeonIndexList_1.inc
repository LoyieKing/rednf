// ---------------------------------------------------------------------------
// CDataManager::getDungeonIndexList()
// ORIG T 0x08365434 sz 0x15（_ZN12CDataManager19getDungeonIndexListEv 非 const）。
// b28 BM4/Main 归位：原定义在 UserPartyLink4.cpp，ORIG 与 CDungeonList::getDungeonIndexList
// 地址相邻（0x0836542c/0x08365434 同 TU）且 callee 定义先见使 4.4 outgoing 精确继承
// （消 0x18 保守帧 → ORIG sub $0x4）。this+0x00 = m_dungeonList，透传返回值。
// （CDungeonList.cpp 为镜像 TU 不入构建，定义落本 TU 才是真实发射点。）
// ---------------------------------------------------------------------------
CDungeonList* CDataManager::getDungeonIndexList()
{
    return m_dungeonList->getDungeonIndexList();
}