// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步（N18a），语义等价。
// 同 IncreasePowerWarPoint。
// [b28d-solo-CUserCharacInfo AE] 36=36：P254 packed enum 视角（EPowerWarPointValue /
// m_powerWarPointEnum）⇒ 减算段两段 regmove 复现。
void CUserCharacInfo::DecreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角（同 IncreasePowerWarPoint）。
    CUR->m_powerWarPointEnum =
        (EPowerWarPointValue)((int)CUR->m_powerWarPointEnum - point);
    if ((short)CUR->m_powerWarPoint < 0)
        CUR->m_powerWarPoint = 0;
    }
}