int DisJoint::_GetAdditionalItemInfo(int rarity)
{
    if (m_additional[rarity].m_count == 0)
    {
        return 0;
    }
    int result = 0;
    const int bound = m_additional[rarity].m_count * 1000;
    result = m_additional[rarity]
        .m_items[get_rand_int(bound) / 1000];
    return result;
