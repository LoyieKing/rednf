std::vector<int>* DisJoint::GetPredictResult(int rarity)
{
    if (rarity < 0 || rarity > 5)
        return &m_resultVecs[0];
    return &m_resultVecs[rarity];
