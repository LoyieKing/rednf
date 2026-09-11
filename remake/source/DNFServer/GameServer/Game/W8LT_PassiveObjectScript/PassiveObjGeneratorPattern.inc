// ORIG 0x08a3ad70：初始化列表 m_b00=1 → m_s04 string → m_i08=0 → m_i0c=0 →
// m_v10 vector<uchar>，函数体 m_s04.clear() 再 m_v10.clear()（含 unwind 清理路径）。
PassiveObjGeneratorPattern::PassiveObjGeneratorPattern()
    : m_b00(1), m_s04(), m_i08(0), m_i0c(0), m_v10()
{
    m_s04.clear();
    m_v10.clear();
}