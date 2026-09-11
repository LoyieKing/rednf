void CGameManager::putBlueMarble(BlueMarble* marble)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x082a2434。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (marble == 0)
        return;
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(marble->getMemoryPoolIndex());
    if (it != m_blueMarbleMap.end())
    {
        m_blueMarbleMap.erase(marble->getMemoryPoolIndex());
        m_blueMarblePool.Free(marble);
    }
}