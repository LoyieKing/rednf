CCargo* CUserCharacInfo::getCurCharacCargoW()
{
    enableSaveCargo();
    return &CUR->m_cargo;
}