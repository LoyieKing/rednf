int BlueMarble::getRandomItem() const
{
    unsigned long range = (unsigned long)m_pScript->m_info.m_itemWeightTotal;
    int r = m_pRands[3].randInt(range);
    // P0：ORIG 无 items 局部槽——每次使用重算 this（0x80d87xx `mov 0x8,%edx;
    // mov 0x38,%edx; add $0x98,%edx`，循环内两处）。
    int count = (int)m_pScript->m_info.m_randomItemList.size();
    int acc = 0;
    // b28c-1：P218 下标 int 声明触发 RHS 值先（ORIG 0x080d7690 先载 i 再算
    //  this 链；unsigned i 恒 this 先）。
    int i = 0;
    while (i < count)
    {
        acc += m_pScript->m_info.m_randomItemList[i].m_weight;
        if (acc >= r)
        {
            return m_pScript->m_info.m_randomItemList[i].m_itemIndex;
        }
        ++i;
    }
    return 0;
