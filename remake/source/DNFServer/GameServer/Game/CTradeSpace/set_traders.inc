// ---- set_traders @ 0x085295f0 ----
void CTradeSpace::set_traders(CUser* a, CUser* b)
{
    lock();
    m_bTrading = 1;
    _SetLock(0);
    // ORIG +0x2c-0x6c：赋值→重读→写 m_padf 交替（无缓存）；
    // m_pTraders[0]=a 写入为 ORIG 实有（+0x32 mov %edx,0x20(%eax)）
    m_pTraders[0] = a;
    m_pTraders[0]->m_padf = (unsigned short)b->get_unique_id();
    m_pTraders[1] = b;
    m_pTraders[1]->m_padf = (unsigned short)a->get_unique_id();
    // ORIG +0x7b-0x11a：&& 短路边 je 假岛、characName 先于 acc_name 求值
    // （R2L 内联实参，P134 无中间槽）；不命名 bool/指针局部避免 spill 帧膨胀
    if (m_pTraders[1]->getCurCharacR() != 0 &&
        m_pTraders[0]->getCurCharacR() != 0) {
        m_pTraders[0]->m_historyLog.SetTrader(
            m_pTraders[1]->get_acc_name(),
            m_pTraders[1]->getCurCharacName());
        m_pTraders[1]->m_historyLog.SetTrader(
            m_pTraders[0]->get_acc_name(),
            m_pTraders[0]->getCurCharacName());
    }
    a->SetTradeSpace(m_index);
    b->SetTradeSpace(m_index);
    m_nMoney[0] = 0;
    m_nMoney[1] = 0;
    m_nState[0] = 0;
    m_nState[1] = 0;
    memset(&m_slotsData[0], 0, 0x798);
    memset(&m_slotsData[1], 0, 0x798);
    memset(m_fieldF68, 0, sizeof(m_fieldF68));
    unlock();
    a->m_historyLog.TradeBegin();
    b->m_historyLog.TradeBegin();
}