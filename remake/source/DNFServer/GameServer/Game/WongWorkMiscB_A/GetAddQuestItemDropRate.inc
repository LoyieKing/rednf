// ---------------------------------------------------------------------------
// CUserPremium::GetAddQuestItemDropRate —— ORIG …23GetAddQuestItemDropRateEi
// W 0x08694a28：level∉[0,0x69] → 0；否则 g_SPremiumInfo[level].m_field48
// （ORIG 基址 0x09483a88 = g_SPremiumInfo + 0x48，元素步长 0x120）。
int CUserPremium::GetAddQuestItemDropRate(int level)
{
    if (level < 0 || level > 0x69)
        return 0;
    return g_SPremiumInfo[level].m_field48;
}