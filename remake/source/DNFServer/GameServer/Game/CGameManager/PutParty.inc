void CGameManager::PutParty(CParty* party)
{
    // N82 封顶（hash_map 迭代器槽平移）：ORIG 槽组 key@-0x18/iter@-0x20/
    // eraseKey@-0x14/erase-arg@-0xc；4.4 对 __gnu_cxx::hash_map find/erase 链
    // 槽组整体 -4 平移（key@-0x14/iter@-0x1c/eraseKey@-0x10/arg@-0x20），
    // 归一后余差仅槽号（fn_diff_tu 实证），槽分配策略跨版本差无源码解。
    // b28d-lane12: 声明序 eraseKey,key 使 key@-0x18 与 ORIG 对齐（ndiff 12→8）；
    // 残余 eraseKey@-0x10 vs ORIG -0xc（end sret 占 -0x14..-0xd）→ N82附。ORIG 0x08294fc8。
    // b28d-solo-CGameManager: AE 达成（compare_tu clean）——P0 内联右值形：ORIG 槽
    // -0x18/-0x14/-0xc 全为 find/erase 的 const& 实参临时与 end() sret 临时（无具名
    // key/eraseKey）；具名标量形把 sret 顶到最浅 -0x10（三形实测 N82附，本波翻正）。
    if (party == 0)
        return;
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it =
            m_partyMap.find(party->GetPartyIndex());
    if (it != m_partyMap.end())
    {
        m_partyMap.erase(party->GetPartyIndex());
        m_partyPool.Free(party);
    }
}