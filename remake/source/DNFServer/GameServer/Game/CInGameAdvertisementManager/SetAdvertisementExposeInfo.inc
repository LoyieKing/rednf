// ORIG W 0x080fdd0a：memcpy(this, &cond, 0x84c)。
void CAdvertisementExposeInfo::SetAdvertisementExposeInfo(
    stInGameADExposeCondition& cond)
{
    std::memcpy(&m_condition, &cond, 0x84c);
}