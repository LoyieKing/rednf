void CGameManager::returnDeathTower(WongWork::CDeathTower* tower)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295852。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (tower == 0)
        return;
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(tower->getIdx());
    if (it != m_deathTowerMap.end())
    {
        m_deathTowerMap.erase(tower->getIdx());
        m_deathTowerPool.Free(tower);
    }
}