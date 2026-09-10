void WeaponInfo::clear()
{
    m_wavs.clear();
    m_hitType = 5;
    m_blood = 0;
    m_field14 = 0;
    m_rate18 = 1.0f;
    m_rate1c = 1.0f;
    m_rate20 = 1.0f;
    for (int i = 0; i < 4; ++i)
        m_skillRates[i] = 1.0f;
