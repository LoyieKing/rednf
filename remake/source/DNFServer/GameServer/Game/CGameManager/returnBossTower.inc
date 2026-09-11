void CGameManager::returnBossTower(WongWork::CBossTower* tower)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295a90。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (tower == 0)
        return;
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(tower->getIdx());
    if (it != m_bossTowerMap.end())
    {
        m_bossTowerMap.erase(tower->getIdx());
        m_bossTowerPool.Free(tower);  // ORIG 0x082b2420 = StaticPool<CBossTower,600>::Free（call 目标核对纠错）
    }
}