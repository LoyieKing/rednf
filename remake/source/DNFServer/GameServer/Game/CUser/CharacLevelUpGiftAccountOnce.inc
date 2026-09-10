CharacLevelUpGiftAccountOnce::CharacLevelUpGiftAccountOnce()
{
    // ORIG 0x086970e6（W）：multimap<int,int> 默认构造后调用 clear()。
    m_map.clear();
