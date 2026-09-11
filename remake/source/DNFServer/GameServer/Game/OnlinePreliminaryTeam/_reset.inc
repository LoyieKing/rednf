// ORIG T 0x08586100：三字段清零（rating 位清 0）。
void COnlinePreliminaryMember::_reset()
{
    m_rating = 0.0f;
    m_winCount = 0;
    m_loseCount = 0;
}