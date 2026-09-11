float ServerParameterScript::GetExpBonusWeight(eRankBonusIndex idx) const
{
    if (idx >= 0 && idx <= 4)
    {
        return m_expBonusWeight3ac[idx];
    }
    return 0.0f;
}