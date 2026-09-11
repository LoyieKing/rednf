unsigned char* CUserCharacInfo::getCurCharacEscaladeTutorialFlag()
{
    return m_selected ? (unsigned char*)CUR->m_tutorialFlags2 : 0;
}