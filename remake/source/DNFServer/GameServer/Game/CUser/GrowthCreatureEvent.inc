GrowthCreatureEvent::GrowthCreatureEvent()
{
    // ORIG 0x086970a4（W）：vector<eventCharacInfo> 默认构造后调用 clear()。
    m_vec.clear();
}