// ---------------------------------------------------------------------------
// 41. CUser::UseMileage @0x08650a10
// ---------------------------------------------------------------------------
void CUser::UseMileage(int cost)
{
    int after = m_mileage - cost;
    LogHistory("Mileage %s,%d,%d,%d", "Use", cost, m_mileage,
               ~(after >> 31) & after);          // max(after,0) 位形态照录
    if (after < 0)
        m_mileage = 0;
    else
        m_mileage = after;
    SaveMileage();                               // NEEDS_DECL(见尾注)
}