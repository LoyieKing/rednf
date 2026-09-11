// ---------------------------------------------------------------------------
// ORIG W 0x085bf940 _ZNK8WongWork12CUserPremium14GetAddExpBonusEi：
// type∈[0,0x69] 时返回 g_SPremiumInfo[type].m_field44（表基 0x09483a84，
// 步长 0x120），否则 0。逐指令转写。
// ---------------------------------------------------------------------------
int CUserPremium::GetAddExpBonus(int type) const
{
    if (type < 0 || type > 0x69)
        return 0;
    return g_SPremiumInfo[type].m_field44;
}