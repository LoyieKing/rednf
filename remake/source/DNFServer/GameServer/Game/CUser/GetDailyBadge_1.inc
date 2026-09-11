// ORIG @0x08568e0c（W）：out[i]=m_dailyBadge[i]，i=0..2
// （ORIG 寻址 (i+0x47040)*2+4 即 m_dailyBadge 逐字读）。
void CUser::GetDailyBadge(short* out)
{
    for (int i = 0; i <= 2; ++i)
        out[i] = m_dailyBadge[i];
}