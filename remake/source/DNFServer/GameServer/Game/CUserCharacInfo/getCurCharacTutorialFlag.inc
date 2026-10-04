int CUserCharacInfo::getCurCharacTutorialFlag()
{
    return m_selected ? m_selected->m_tutorialFlag : 0;
}