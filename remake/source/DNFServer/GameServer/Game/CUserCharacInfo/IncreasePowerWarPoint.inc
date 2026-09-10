void CUserCharacInfo::IncreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角：复现 ORIG `movzwl 0x121f(%edx),%edx; mov %edx,%ecx` 两段。
    CUR->m_powerWarPointEnum =
        (EPowerWarPointValue)((int)CUR->m_powerWarPointEnum + point);
    if ((short)CUR->m_powerWarPoint > 9999)
        CUR->m_powerWarPoint = 9999;
    }
