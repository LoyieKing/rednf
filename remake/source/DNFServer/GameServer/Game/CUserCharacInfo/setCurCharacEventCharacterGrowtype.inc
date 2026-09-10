void CUserCharacInfo::setCurCharacEventCharacterGrowtype(char growtype)
{
    enableSaveCharacEtc();
    CUR->m_eventCharacterGrowtype = (unsigned char)growtype;
