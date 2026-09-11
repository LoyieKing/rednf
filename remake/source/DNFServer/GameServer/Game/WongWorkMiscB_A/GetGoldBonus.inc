// ---------------------------------------------------------------------------
// CUserPremium::GetGoldBonus —— ORIG _ZN8WongWork12CUserPremium12GetGoldBonusEi
// W 0x08694a64（非 const）：this+0x8bc 字节非 0 → level*level*100，否则 0。
int CUserPremium::GetGoldBonus(int level)
{
    if (m_field8bc != 0)
        return level * level * 100;
    return 0;
}