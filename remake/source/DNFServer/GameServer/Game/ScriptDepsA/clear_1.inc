void CharacterEtcStats::clear()
{
    m_category.clear();
    for (int i = 0; i < 4; ++i)
        m_moduleDamageRate[i] = 0.0f;
