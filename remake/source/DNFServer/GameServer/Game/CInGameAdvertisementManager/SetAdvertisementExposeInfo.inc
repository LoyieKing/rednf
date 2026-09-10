void CAdvertisementExposeInfo::SetAdvertisementExposeInfo(
    stInGameADExposeCondition& cond)
{
    std::memcpy(&m_condition, &cond, 0x84c);
