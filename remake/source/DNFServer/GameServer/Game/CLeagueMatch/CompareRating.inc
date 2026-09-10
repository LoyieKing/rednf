bool CompareRating(PlayerInfo const& a, PlayerInfo const& b)  // ORIG T 0x0855c952
{
    // b28d-lane6：m_flag bool 化（P248 档②）复现第一段 ORIG 0x0855c95a 双重 movzbl
    // + cmpl/jle；第二段改 `==` 正向包裹复现 `jne` 落 false 尾块（P222/P47）；
    // rating 段用 if/return true 保留 ORIG 的 seta/test/je 物化（P56）。
    if (a.m_flag > b.m_flag) return true;
    if (a.m_flag == b.m_flag)
    {
        if (a.m_rating > b.m_rating) return true;
    }
    return false;
