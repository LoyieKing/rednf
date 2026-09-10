unsigned char CGameOption::IsEtcOptionChanged(CETCOption& EtcOption)
{
    int nRet = memcmp(&m_EtcOption, &EtcOption, 0x48);
    if (nRet == 0)
        return 0;
    return 1;
