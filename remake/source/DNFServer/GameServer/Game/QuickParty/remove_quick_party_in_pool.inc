// remove_quick_party_in_pool —— ORIG 0x0826aa9a。
// 报告：docs/class_func_reports/QuickParty__CQuickPartySystemManager/remove_quick_party_in_pool.md
// pool index ∈ [0,3)；map 未命中 → destroy + trace 并返回 false；
// 命中 → 从对应档位 list 移除 quick party index 并返回 true。
// [b28d-L1] N93/N98 封顶：ORIG 0x0826aa9a 槽序 it=-0x28/end 临时=-0x24/qpIndex=-0x20；块内声明使 4.4 把 qpIndex 排最深。P212 全头部声明已把 it 翻正 -0x28（ndiff 52→46），残余 end 临时槽与 qpIndex 互换——声明序/具名 e 两形实测（r19c/r19d）只交换槽位，4.4 按首次使用序分配不可翻。
// [b28d-solo-QuickParty] verdict=AE（71=71，TU routed 口径；P167 CLEAR）：修法=
// qpIndex 具名局部改内联实参 `m_lists[poolIdx].remove(qp->get_quick_party_index())`
// ——ORIG 0x0826ab40 的 qpIndex 槽实为 list::remove(const int&) 绑定的匿名临时
// （-0x20），end() sret 临时落 -0x24；原具名局部使 4.4 把两者槽号互换。旧
// N93/N98 封顶判词作废。
bool CQuickPartySystemManager::remove_quick_party_in_pool(
    CQuickParty* qp, const STQuickPartyPoolMap_Key& key)
{
    if (qp == 0)
        return false;

    char poolIdx = qp->get_pool_index();
    if (poolIdx >= 4 || poolIdx < 0)
        return false;

    // [b28d-L1] 修法（P212 全头部声明，ndiff 52→46 为四形最优）：ORIG 槽序
    // it=-0x28 / end 临时=-0x24 / qpIndex=-0x20。块内声明 qpIndex 时 4.4 把它排到
    // 最深 -0x28、it/end 各上移一格；把 qpIndex 提到 it 之前声明（块内仅赋值）后
    // it 回到 -0x28（与 ORIG 全等），残余仅 end 临时槽 -0x20 与 qpIndex -0x24 互换
    // ——4.4 按首次使用序分配（N98），end 条件先于块内赋值使用，声明序/具名 e 两形
    // 实测（/tmp/b28d_l1/r19c.md 52、r19d.md 48）均只交换槽位，判 N93/N98 封顶。
    std::map<STQuickPartyPoolMap_Key, STQuickPartyPoolMap_Data>::iterator it =
        m_poolMap.find(key);
    if (it != m_poolMap.end())
    {
        it->second.m_lists[poolIdx].remove(qp->get_quick_party_index());
        return true;
    }

    qp->destroy();
    // b28c-lane3：P221 全表达式临时链（单次调用）。
    cMyTrace(__PRETTY_FUNCTION__, 0x2ba, 5)(
        "QUICK_PARTY_LOG : NO EXIST KEY DATA!!");
    return false;
}