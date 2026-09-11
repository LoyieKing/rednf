void CClearReward::setDropRatio(float ratio)
{
    for (int grp = 0; grp <= 5; ++grp)      // ORIG 0x08538687 setle（有符号 <=5）
    {
        m_workProbs[grp] = m_scriptProbs[grp];
        for (size_t i = 0; i < m_workProbs[grp].size(); ++i)
        {
            // ORIG 085385c8：roundFloat(value * ratio) 回写工作副本。
            // 单次 operator[] 后经具名引用读两处（P157 例外；P167 原报 EXTRA ×1）。
            stGenItemProb_t& prob = m_workProbs[grp][i];
            prob.v[2] =
                static_cast<int>(roundFloat(static_cast<float>(prob.v[2]) * ratio));
        }
    }
}