void CUser::GetDailyBadge(short* out)
{
    for (int i = 0; i <= 2; ++i)
        out[i] = m_dailyBadge[i];
