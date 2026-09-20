CCargo* CUserCharacInfo::getCurCharacCargoW()
{
    enableSaveCargo();
    return &m_selected->m_cargo;
}
