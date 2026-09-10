void CUser::AddDailyBadge(ENUM_BADGE_TYPE badge)
{
    // ORIG 0x08568e5e：单装载 movzwl + lea 0x1(%edx) + 单 store —— 后缀 ++（技巧4）
    m_dailyBadge[(int)badge]++;
