// ----------------------------------------------------------------------------
// stDollControllerResult — C1/C2 W 0x0896deac range [...dee5]
//   w+8=0 → w+6=w+8 → w+4=w+6 → d+0=(uint)(word)(w+4)   // 链式置零
// D1/D2 W 0x089751fa size 5（空体）
// ----------------------------------------------------------------------------
STExpertJobScript::stDollControllerResult::stDollControllerResult()
{
    ZERO_WORD(&m_field8);                       // +8 word = 0
    m_field6 = m_field8;                        // +6 word = w+8
    m_field4 = m_field6;                        // +4 word = w+6
    m_field0 = static_cast<int>(
        static_cast<unsigned short>(m_field4)); // +0 dword = zext(w+4)
}