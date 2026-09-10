void CVendingMachineStatistic::addOutputItem(unsigned long itemIdx,
                                             unsigned int count)
{
    // ORIG：若 itemIdx 属于 m_usedItems（追踪类型之一），则在 m_output 中
    // 累加 count；否则 ignore（直接返回）。
    std::vector<unsigned long>::iterator found =
        std::find(m_usedItems.begin(), m_usedItems.end(), itemIdx);
    if (found == m_usedItems.end())
        return;

    std::map<unsigned long, unsigned int>::iterator it = m_output.find(itemIdx);
    // [b28c-31] ORIG 0x0860f6xx：`call eq; test %al,%al; je <+= 块>`（eq 真即
    // it==end 时 fall-through 到 insert 块）⇒ 源码须写 `== end` 正向形。
    if (it == m_output.end())
        m_output.insert(std::make_pair(itemIdx, count));
    else
        it->second += count;
