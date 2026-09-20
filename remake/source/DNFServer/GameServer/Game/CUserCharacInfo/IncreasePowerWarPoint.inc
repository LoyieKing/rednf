// ORIG `movzwl 0x121f(%edx),%edx;mov %edx,%ecx` 二段；4.4 折单步 `movzwl …,%ecx`
// （N18a：4.4 恒折 mem→%reg→callee-saved 两段），寄存器分配差异，语义等价。
// [b28d-solo-CUserCharacInfo N18a 复核] `+=` 形实测同形（movzwl mem,%ecx 单段）；
// 字段为 16 位 ushort，无法按 P254 改 enum（宽度变 4 破坏 +0x121f 布局）⇒ 封顶。
void CUserCharacInfo::IncreasePowerWarPoint(short point)
{
    if (m_selected)
    {
    enableSaveCharacStat();
    // P254 packed enum 视角：复现 ORIG `movzwl 0x121f(%edx),%edx; mov %edx,%ecx` 两段。
    m_selected->m_field121f =
        (EPowerWarPointValue)((int)m_selected->m_field121f + point);
    if ((short)m_selected->m_field121f > 9999)
        m_selected->m_field121f = 9999;
    }
}