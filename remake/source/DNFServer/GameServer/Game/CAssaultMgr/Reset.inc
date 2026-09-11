void CAssaulter::Reset()               // ORIG W 0x82f05ba
{
    m_user = 0;
    m_team = (ENUM_ASSAULT_TEAM)0;
    m_state = (ENUM_ASSAULT_STATE)0;
    m_lastHpSec = 0;
    m_powerWarWinner = 0;
}