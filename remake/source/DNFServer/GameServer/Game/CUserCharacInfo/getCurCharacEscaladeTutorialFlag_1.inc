unsigned char* CUserCharacInfo::getCurCharacEscaladeTutorialFlag()
{
    return m_selected ? (unsigned char*)m_selected->m_tradeRestrictData : 0;
}