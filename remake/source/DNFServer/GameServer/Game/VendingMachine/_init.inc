// ----------------------------------------------------------------------------
// _init（ORIG 0x086105ac）
// ----------------------------------------------------------------------------
void CVendingMachineStatistic::_init()
{
    // ORIG：m_useCount=0；m_usedItems 追加 0x1d1b/0x1d1c/0x1d1d 三项；
    // m_output.clear()。
    m_useCount = 0;
    m_usedItems.push_back(0x1d1b);   // 追踪的售货机物品类型索引（ORIG 3 项）
    m_usedItems.push_back(0x1d1c);
    m_usedItems.push_back(0x1d1d);
    m_output.clear();
}