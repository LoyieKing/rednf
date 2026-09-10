int CRandomValue::operator()()
{
    int value = m_a;
    for (int i = 0; i < m_tryCount; ++i)
    {
        // [L27-r4 已实测] ORIG 段无具名 r 槽（我方多 `mov %eax,-0xc(%ebp)` + 一次 m_a 重载），
        //   但把 std::rand() 内联进表达式**反劣**（75→80：4.4 提前物化 m_a 到 %ebx 并
        //   增 push %esi / 帧 0x34→0x30）⇒ 维持具名 r（P329/P183 单向性族）。
        int r = std::rand();
        value = m_a + r % (m_b - m_a + 1);
        if (std::find(m_list.begin(), m_list.end(), value) == m_list.end())
            break;
    }
    return value;
