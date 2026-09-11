// ORIG T 0x085fddb2：问题四元组合法性检查后导出（越界/全零则重置为 0,0,1,1）。
void WongWork::CSecurityCard::generateQuestion(short& a, short& b, short& c,
                                               short& d)
{
    // [b28f-SweepGroupR3 判据 P0/P183] ORIG 0x085fddb2 全程**无具名局部槽**：4 个字段
    //   各自 `movzbl 0x3c+i(%eax),%eax; test %al,%al; jne <T>` 直测（widthscan：
    //   我方 clr_b our=2/cmp_b our=14 vs ORIG movzbl 16 处）⇒ 原码把 4 个 char
    //   与两个 bool **全部内联**到表达式/条件；原写的 q0..q3 + allZero/outOfRange
    //   具名局部各占槽并令条件退化为 `cmpb $0x0,-0xN(%ebp)`。
    if ((m_question3c[0] == 0 && m_question3c[1] == 0 &&
         m_question3c[2] == 0 && m_question3c[3] == 0) ||
        ((m_question3c[0] < 0 || m_question3c[0] > 9) ||
         (m_question3c[1] < 0 || m_question3c[1] > 3) ||
         (m_question3c[2] < 0 || m_question3c[2] > 9) ||
         (m_question3c[3] < 0 || m_question3c[3] > 3)))
    {
        m_question3c[0] = 0;
        m_question3c[1] = 0;
        m_question3c[2] = 1;
        m_question3c[3] = 1;
    }
    a = (short)m_question3c[0];
    b = (short)m_question3c[1];
    c = (short)m_question3c[2];
    d = (short)m_question3c[3];
}