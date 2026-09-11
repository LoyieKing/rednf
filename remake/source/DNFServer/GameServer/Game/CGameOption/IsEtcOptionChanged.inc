// [Q4DispatchBig 2026-09-11] bool 轴（定义侧拼写需与声明同改）
bool CGameOption::IsEtcOptionChanged(CETCOption& EtcOption)
{
    int nRet = memcmp(&m_EtcOption, &EtcOption, 0x48);
    if (nRet == 0)
        return 0;
    return 1;
}