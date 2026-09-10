void AdvanceAltarData::reset()
{
    std::memset((char*)this, 0, 0x21);
    m_update = 0;
    m_stages.clear();
    m_buyItems1.clear();
    m_buyItems2.clear();
    m_buyItems3.clear();
    m_slots1.clear();
    m_slots2.clear();
    m_rewards.clear();
