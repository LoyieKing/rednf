void CUserCharacInfo::setCurCharacEventCharacterGrowtype(char growtype)
{
    enableSaveCharacEtc();
    m_selected->m_field123f = (unsigned char)growtype;
}