bool CDataManager::initRandomOptionItem()
{
    // b28 BM4 判词（N18a/N4 在册）：fresh 实残 = make_pair 首 ref 实参 ORIG
    // `lea 0x4(%eax),%ecx` 单条折叠 vs 4.4 `add $0x4,%eax; mov %eax,%edx`
    // 两段+中转 hop（+1 行）；P30 穷举 add/lea 与寄存器名不可齐。其余
    // 迭代器/插入流逐条同形（含 ++count 逗号序）。语义等价封顶。
    int count = 0;
    for (std::map<int, std::map<int, BaseItemValue> >::iterator outerIt =
             ((CDataManagerBaseItemView*)this)->m_baseItemInfo.begin();
         outerIt !=
             ((CDataManagerBaseItemView*)this)->m_baseItemInfo.end();
         ++outerIt)
    {
        count = 0;
        // ORIG：++innerIt 先于 ++count 发射 ⇒ ++count 位于 for-third（逗号序）。
        for (std::map<int, BaseItemValue>::iterator innerIt =
                 outerIt->second.begin();
             innerIt != outerIt->second.end(); ++innerIt, ++count)
        {
            if (!((CDataManagerBaseItemView*)this)
                     ->m_lookupIndex.insert(
                         std::make_pair(
                             (unsigned long&)innerIt->second.m_field0,
                             count)).second)
            {
                return 0;
            }
        }
    }
    return 1;
