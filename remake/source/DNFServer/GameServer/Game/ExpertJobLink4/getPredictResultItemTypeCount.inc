int DisJoint::getPredictResultItemTypeCount(int rarity) const
{
    if (rarity < 0 || rarity > 5)
        return 1;
    if (m_additional[rarity].m_count != 0)
        return 2;
    return 1;
