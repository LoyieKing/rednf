// ORIG T 0x08131660（docs/class_func_reports/advancealtar__CharacAdvanceAltarManager/
// reset.md 全文）：current(+0x00)=0；循环 local_10<=0 一次 reset +i*0xd0+4 处 AdvanceAltarData。
void CharacAdvanceAltarManager::reset()
{
    m_currentAdvanceAltarData = 0;
    for (int local_c = 0; local_c <= 0; ++local_c)
    {
        // 单实例布局：m_data@+0x04、stride 0xd0 == ORIG `this + local_c*0xd0 + 4`。
        m_data[local_c].reset();
    }
}