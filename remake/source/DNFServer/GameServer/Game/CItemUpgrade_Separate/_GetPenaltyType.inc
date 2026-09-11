// ORIG T 0x0811e2ce：恒返回 1（报告全文仅此）。
int CItemUpgrade_Separate::_GetPenaltyType(unsigned char /*a*/, ENUM_RARITY /*rarity*/,
                                           const GenuineDamageUpgradeTable& /*table*/,
                                           const GenuineDamageUpgradeInfo* /*info*/)
{
    return 1;
}