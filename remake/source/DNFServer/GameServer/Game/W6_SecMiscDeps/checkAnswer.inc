// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N4/P30 + P235）→ 试=新口径 n1_v 66/67 gap=-1；前任两形（`(&m_table[0])[idx]` 数组衰减逐条不变；`(uchar*)(this)+8` 真指针下标 66→71 劣化已回滚）+ 本波复核不变 ⇒ 维持（P235 正解需在 CUser.h:1185 CSecurityCard +0x08 具名化答案表，属共享头 —— 见交付 NEEDS-HEADER）。
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
    // [M2FreeNext15A · 2026-09-11 复核] our 66 = orig 66，残余两处：
    // ①ORIG `add %eax,%edx`（索引累加到 edx）vs 4.4 `lea (%edx,%eax,1),%eax`；
    // ②我方多 `mov 0x8(%ebp),%ecx`（视图 base 先载，P235）而 ORIG 在 add 后
    // 重新 `mov 0x8(%ebp),%eax` 作 base。P235 正解需在权威类 CSecurityCard
    // （CUser.h:1185）+0x08 处具名化答案表成员（现为 TU 局部视图 cast）⇒
    // 属共享头改动，未自行落地，见交付 NEEDS-HEADER。
    // [N1A 2026-09-11 负结论 · 封顶] 两试均不可造 ORIG 形：
    // ①`(&...->m_table[0])[idx]`（数组→指针衰减）：逐条不变（4.4 归一回 ARRAY_REF）；
    // ②`(reinterpret_cast<const unsigned char*>(this) + 8)[idx]`（真指针下标，期望
    //   save_expr(index) 走 NORMAL 展开 ⇒ ORIG 0x085fdfce `add %eax,%edx`）：
    //   实测反劣 our 66→71（新增 `add $0x8,%eax; lea (%ecx,%eax,1),%eax`，disp 8
    //   不再折叠）⇒ 回滚。残余 2 处（ORIG `add %eax,%edx`+`mov 0x8(%ebp),%eax`
    //   重载 base / 我方 `lea (%edx,%eax,1),%eax`+提前 `mov 0x8(%ebp),%ecx`）
    //   系 4.4 地址上下文固有寄存器选择 ⇒ gap −1 封顶。
    if (m_question3c[0] == 0 && m_question3c[1] == 0 &&
        m_question3c[2] == 0 && m_question3c[3] == 0)
    {
        return false;
    }
    if (static_cast<signed char>(
            m_serial[
                static_cast<signed char>(m_question3c[1]) * 10 +
                static_cast<signed char>(m_question3c[0])]) ==
        static_cast<int>(ansA))
    {
        if (static_cast<signed char>(
                m_serial[
                    static_cast<signed char>(m_question3c[3]) * 10 +
                    static_cast<signed char>(m_question3c[2])]) ==
            static_cast<int>(ansB))
        {
            return true;
        }
    }
    return false;
}