void CUserCharacInfo::setCurCharac(_Charac_info* p)
{
    enableSaveCharacInfo();
    enableSaveCharacStat();
    enableSaveCharacEtc();
    m_selected = p;
}