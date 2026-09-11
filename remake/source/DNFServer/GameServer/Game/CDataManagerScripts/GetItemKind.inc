// ============================================================================
// GetItemKind（ORIG T 0x0890d1c0，sret stTempRewardInfo*）：
// m_npcEventMap.find(idx) 未命中 → 默认值；遍历 node->m_favor[kind].m_items
// （kind=0..2，步长 0x14）：item.m_itemId==itemId 且 item.m_count<=count：
//   count==0 → 默认值（不写）；
//   否则 → out.m_kind=kind、out.m_count=ROUND(count/item.m_count)。
// [N类·跨版本豁免] 已还原 ORIG 真形（our 113 vs 112）：残 1 指令差为 gcc4.1/4.4 的
// outgoing-frame / 死栈槽产物（范式库『跨版本』），助记符序列已对齐；语义完全一致。
// ============================================================================
// [b28d-solo-CDataManagerScripts] AE：p167 CLEAR（7/7）。本轮两处翻正：
// ①`stNPCEventItemInfo::m_count` int→unsigned int（CDataManager.h，P254 档③）——
//   复现 ORIG 0x0890d2a8 `mov $0,%edx; mov %eax,-0x38; mov %edx,-0x34; fildll`
//   的无符号零扩展（旧 int 字段产 `fildl`）；②相等比较按 ORIG 操作数序写
//   `itemId == (unsigned int)item.m_itemId` ⇒ 复现 `mov -0x24(%ebp),%eax;
//   cmp 0x14(%ebp),%eax`（旧写法多一条形参装载）。帧/全部槽号与 ORIG 逐一对齐；ORIG 0x0890d1c0。
stTempRewardInfo CNPCRelationEventManager::GetItemKind(
    unsigned short idx, unsigned int itemId, unsigned int count)
{
    stTempRewardInfo result;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;   // ORIG 0x0890d1d2 `mov %ebx,(%esp); call C1Ev` ⇒ NRVO 直构 sret

    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d1c0：不绑定 vector 引用/迭代器，每轮 size()/at() 现取
    // &node->m_favor[kind].m_items（m_items 在 favor[kind]+0，步长 0x14），
    // at() 结果缓存一次后读 m_itemId(+0)/m_count(+4)；ROUND=(int)(long double)
    // count/(long double)m_count（x87 fildll/fdivrp/fldcw-PC64/fistpl 最近舍入）。
    for (int kind = 0; kind < 3; ++kind)
    {
        for (unsigned int col = 0;
             col < node->m_favor[kind].m_items.size(); ++col)
        {
            stNPCEventItemInfo item = node->m_favor[kind].m_items.at(col);
            // ORIG 0x0890d277：`mov -0x24(%ebp),%eax; cmp 0x14(%ebp),%eax; jne`
            // （形参在 mem 源、字段入 eax）⇒ 相等比较按 ORIG 操作数序书写。
            if (itemId == (unsigned int)item.m_itemId
                && (unsigned int)item.m_count <= count)
            {
                if (item.m_count == 0)
                    return result;
                result.m_kind = kind;
                result.m_count =
                    (int)((long double)count / (long double)item.m_count);
                return result;
            }
        }
    }
    return result;
}