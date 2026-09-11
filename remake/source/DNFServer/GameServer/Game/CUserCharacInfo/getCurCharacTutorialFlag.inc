int CUserCharacInfo::getCurCharacTutorialFlag()
{
    return m_selected ? CUR->m_tutorialFlags : 0;
}