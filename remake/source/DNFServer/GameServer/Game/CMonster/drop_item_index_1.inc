int CMonster::drop_item_index(unsigned int index, int randomValue) const
{
    // ORIG 0x0834f7e4：同单参版，空分支直接用传入 randomValue；两分支均
    // tail-jmp 保留 eax = _drop_item_index 返回值（调用点取回判 != -2）。
    if (!m_dropItems.empty())
        return _drop_item_index(get_rand_int(m_specialItemWeight), m_dropItems);
    else
        return _drop_item_index(randomValue, m_specialItemLists[index]);
