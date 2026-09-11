void CUserCharacInfo::resetCharacTutorialFlag()
{
    if (m_selected)
    {
    enableSaveCharacStat();
    CUR->m_tutorialFlags = 0;
    memset(CUR->m_tutorialFlags2, 0, 0x10);
    }
}