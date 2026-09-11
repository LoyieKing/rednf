void CBattle_Field::CBloodClearRewardData::addPlayTime(unsigned int t)
{
    m_prevPlayTime = m_playTime;
    m_playTime = m_playTime + t;
}