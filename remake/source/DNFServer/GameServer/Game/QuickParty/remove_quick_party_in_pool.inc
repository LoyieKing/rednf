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
