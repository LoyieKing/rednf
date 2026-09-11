bool Arad_StatisticsEventItemList::loadScriptData()
{
    // b28d-solo-Arad_StatisticsEventItemList: AE（.o 阶段上限；call 目标已对齐）。
    // ORIG 0x0819cd2c：30 个字面量右值绑定 push_back(unsigned long&&)（EOm 右值重载）；
    // 每个右值物化到独立栈槽（-0x80 … -0xc）后取址传参（P0 限定①：匿名临时槽）。
    // 旧形为 30 个具名局部左值 ⇒ 选 push_back(const unsigned long&)（ERKm），
    // 除 30 条 call 目标外逐条全等（ndiff=30）；改右值后 TU diff=0、p167 30/30 CLEAR。
    // 反例记录：static_cast<unsigned long&&>(具名局部) 会被 4.4 多物化一份临时槽
    // （帧 0x98→0x108、槽 -0xf8… 全漂），不可用；std::move 会引入 ORIG 无的 call（P234）。
    m_statisticsItemList.push_back(0x28d3aa);
    m_statisticsItemList.push_back(0x28d3a4);
    m_statisticsItemList.push_back(0x6856);
    m_statisticsItemList.push_back(0x6857);
    m_statisticsItemList.push_back(0x6858);
    m_statisticsItemList.push_back(0x6859);
    m_statisticsItemList.push_back(0x685a);
    m_statisticsItemList.push_back(0x685b);
    m_statisticsItemList.push_back(0x685c);
    m_statisticsItemList.push_back(0x1d98);
    m_statisticsItemList.push_back(0x28d435);
    m_statisticsItemList.push_back(0x28d436);
    m_statisticsItemList.push_back(0x28d437);
    m_statisticsItemList.push_back(0x28d438);
    m_statisticsItemList.push_back(0x28d439);
    m_statisticsItemList.push_back(0x1d95);
    m_statisticsItemList.push_back(0x28d14d);
    m_statisticsItemList.push_back(0x28cdad);
    m_statisticsItemList.push_back(0x28cdac);
    m_statisticsItemList.push_back(0x28d487);
    m_statisticsItemList.push_back(0x28d4bf);
    m_statisticsItemList.push_back(0x2a445c);
    m_statisticsItemList.push_back(0x2a4a9c);
    m_statisticsItemList.push_back(0x2a7026);
    m_statisticsItemList.push_back(0x2a639c);
    m_statisticsItemList.push_back(0x2a50dc);
    m_statisticsItemList.push_back(0x2a571c);
    m_statisticsItemList.push_back(0x2a7660);
    m_statisticsItemList.push_back(0x2a5d63);
    m_statisticsItemList.push_back(0x2a69dd);
    return true;
}