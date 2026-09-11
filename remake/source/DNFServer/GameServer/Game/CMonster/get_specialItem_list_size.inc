unsigned int CMonster::get_specialItem_list_size(unsigned int index) const
{
    // ORIG 0x0834f746：cmpl $0x3; ja 跳 return 0，fall-through 为 size()。
    if (index <= 3)
        return static_cast<unsigned int>(m_specialItemLists[index].size());
    return 0;
}