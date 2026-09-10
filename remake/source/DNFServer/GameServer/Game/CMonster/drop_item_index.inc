void CMonster::drop_item_index(unsigned int index) const
{
    // ORIG 0x0834f776：m_dropItems(+0x3c) 非空 → get_rand_int(m_specialItemWeight@+0x38)
    // 对 m_dropItems；空 → specialItemLists[index](+0x8)。无 index 检查。
    if (!m_dropItems.empty())
        _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
    else
        _drop_item_index(get_rand_int(10000), m_specialItemLists[index]);
