// ORIG 5 参 0x854f746：反汇编实测 0x18(%ebp)=第 5 参(bFieldC)→+0x0d、
// 0x1c(%ebp)=第 6 参(bFieldD)→+0x0c——**字段与形参名交叉**（m_field_c 存的是
// bFieldD，m_field_d 存的是 bFieldC）。冒烟实测 logInit 的 bTimestamp 因此差
// 异（ORIG 输出行首是否带CRLF），已按ORIG实测映射修正。
cMyTrace::cMyTrace(const char* name, int line, int flag, bool bFieldC, bool bFieldD) throw()   // [Main 全局 A/B 实验]
{
    m_name = name;
    m_line = line;
    m_flag = flag;
    // [b28c-35] 字段已 bool 化（GameTypes.h）⇒ 直赋（P211 同型 LHS-first）；旧
    // `(char)` cast 产值先形（ORIG 0x0854f746 尾 store this 先）。
    m_field_c = bFieldD;   // ORIG +0x0c ← 第 6 参 bFieldD
    m_field_d = bFieldC;   // ORIG +0x0d ← 第 5 参 bFieldC
}