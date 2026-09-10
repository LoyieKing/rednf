void CSpecialMonsterDrop::setDropRatio(float ratio)
{
    // [b28d-solo-WongWorkDrop_B] AE：51=51（同 Hell 版；残差仅重定位标签）。
    // [b28d-38] 同 CMonsterDrop_Hell::setDropRatio（ORIG 两份同型物理独立块）。
    m_probsSub = m_probs;
    for (size_t i = 0; i < m_probsSub.size(); ++i)
    {
        stGenItemProb_t& e = m_probsSub[i];
        for (int j = 0; j < 5; ++j)
        {
            e.w[j] = static_cast<int>(roundFloat(static_cast<float>(e.w[j]) * ratio));
        }
    }
