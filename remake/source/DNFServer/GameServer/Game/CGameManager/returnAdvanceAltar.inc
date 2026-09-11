void CGameManager::returnAdvanceAltar(advancealtar::StageControl* control)
{
    // b28d-lane12: 同 PutParty——ndiff 12→8；残余 eraseKey@-0x10 vs -0xc → N82附。
    // ORIG 0x08295cce。
    // b28d-solo-CGameManager: AE 达成——P0 内联右值形（ORIG 槽全为实参/sret 临时）。
    if (control == 0)
        return;
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(control->getIndex());
    if (it != m_stageMap.end())
    {
        m_stageMap.erase(control->getIndex());
        m_stagePool.Free(control);
    }
}