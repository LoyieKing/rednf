long CMonster::deathTowerSpecifyItemDrop() const
{
    // ORIG 0x0834f84a：无 empty 检查，直接 get_rand_int(10000) 后转发。
    return _drop_item_index(get_rand_int(10000), m_deathTowerItems);
}