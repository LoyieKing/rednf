void CUserCharacInfo::resetCharacTutorialFlag()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    m_selected->m_tutorialFlag = 0;
    memset(m_selected->m_tradeRestrictData, 0, 0x10);
    }
}