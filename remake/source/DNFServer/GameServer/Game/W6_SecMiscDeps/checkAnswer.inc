bool WongWork::CSecurityCard::checkAnswer(unsigned char ansA, unsigned char ansB) const
{
    // [b28d-solo-W6_SecMiscDeps 判词 · N 封顶（P30/N4 add-vs-lea + P235 视图 base 先载）]
    // ORIG 0x085fdf6a：帧 0x8，
    // 形参 spill -0x4/-0x8；rowA/colA/rowB/colB 与 table 指针**无槽往返**
    // （ORIG `movzbl 0x3d(%eax); movsbl %al,%edx; …; movzbl 0x8(%eax,%edx,1)`）
    // ⇒ 原码全部内联表达式 + 表按 this+8 具名数组视角访问（disp 折叠）；
    // 比较用 P222 正向包裹（ORIG `jne <共享 false 尾块>`）。
    // ORIG W 0x085fdf6a (size 0xbb)。问题四元组 @0x3c..0x3f；答案表基址 this+8，
    // 第一问索引 = question[1]*10 + question[0]，第二问 = question[3]*10 +
    // question[2]；表值与答案均经 movsbl/zbl 后整数值比较。
    if (m_question3c[0] == 0 && m_question3c[1] == 0 &&
        m_question3c[2] == 0 && m_question3c[3] == 0)
    {
        return false;
    }
    if (static_cast<signed char>(
            reinterpret_cast<const CSecurityCardAnswerView*>(this)->m_table[
                static_cast<signed char>(m_question3c[1]) * 10 +
                static_cast<signed char>(m_question3c[0])]) ==
        static_cast<int>(ansA))
    {
        if (static_cast<signed char>(
                reinterpret_cast<const CSecurityCardAnswerView*>(this)->m_table[
                    static_cast<signed char>(m_question3c[3]) * 10 +
                    static_cast<signed char>(m_question3c[2])]) ==
            static_cast<int>(ansB))
        {
            return true;
        }
    }
    return false;
