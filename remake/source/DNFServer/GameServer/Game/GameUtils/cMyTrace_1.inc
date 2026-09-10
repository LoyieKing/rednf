cMyTrace::cMyTrace(const char* name, int line, int flag, bool bFieldC, bool bFieldD)
{
    m_name = name;
    m_line = line;
    m_flag = flag;
    // [b28c-35] 字段已 bool 化（GameTypes.h）⇒ 直赋（P211 同型 LHS-first）；旧
    // `(char)` cast 产值先形（ORIG 0x0854f746 尾 store this 先）。
    m_field_c = bFieldD;   // ORIG +0x0c ← 第 6 参 bFieldD
    m_field_d = bFieldC;   // ORIG +0x0d ← 第 5 参 bFieldC
