void CUserCharacInfo::setCurCharacEventCharacterGrowtype(char growtype)
{
    enableSaveCharacEtc();
    m_selected->m_eventCharacterGrowtype = (unsigned char)growtype;
}