int CHARAC_LOAD_MERCENARY::GetCompetitionPeriodTerm(bool* isFinalPeriod) const
{
    if (!CheckCompetitionPeriodIndex(static_cast<char>(m_field31)))
        return 0;
    const std::vector<std::pair<int, float> >& vec =
        G_CDataManager()->m_mercenarySystem.m_vec14;
    // ORIG 0x081b2f76：`call empty; test; jne L1; call size; cmp %al,%dl; jge L2;
    // L1: mov $1; L2: mov $0; test; je body` —— 8 位有符号比较 + 无具名 bool
    // （P158 反向：具名 bool 产 -0xd 槽往返，ORIG 用 %eax 直接物化）。
    if (vec.empty() ||
        static_cast<char>(vec.size()) < static_cast<char>(m_field31))
        return 0;
    // ORIG 0x081b2fcb：分子先落 -0xc 槽（`mov %eax,-0xc` + 回读 →%ebx），除法
    // 后再写回同一槽 ⇒ 两句式（P38 拆语句）；`divl`（无符号）⇒ 商按 unsigned 求。
    // [b28d-solo-CUserMissingA2②] P226 条件/实参向语句表达式：ORIG 0x081b2fa5 段
    // 先求 getCurSec（call→eax→mov edx,eax）再读 m_field28，4.4 恒先读内存
    // ⇒ 裸语句表达式强制左操作数先物化。
    int term = ({ GlobalData::s_systemTime_.getCurSec(); }) - m_field28;
    term = static_cast<int>(
        static_cast<unsigned int>(term)
        / static_cast<unsigned int>(
            G_CDataManager()->m_mercenarySystem.m_field0));
    if (term < vec[static_cast<int>(static_cast<char>(m_field31))].first)
    {
        if (isFinalPeriod != 0)
            *isFinalPeriod = true;
    }
    else
    {
        term = vec[static_cast<int>(static_cast<char>(m_field31))].first;
    }
    return term;
