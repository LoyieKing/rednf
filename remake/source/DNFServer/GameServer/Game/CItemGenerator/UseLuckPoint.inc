// ============================================================================
// ::CLuckPoint::UseLuckPoint —— ORIG T 0x08550a84 逐指令转写：
//   user 为空 / itemType!=1 / rarity!=2 → 返回 0；
//   level = user->get_charac_level()（第一次）；diff = grade - level；
//   rate = G_CDataManager()->m_serverParameter（+0x68 实证）
//   .getLuckPointDeductionRate(第二次 get_charac_level（P48 无 CSE）,
//                              abs(diff)（sar $0x1f/xor/sub 惯用法，内联实参）)；
//   user->DecCurCharacLuckPoint(rate)；返回 1。
// ============================================================================
bool CLuckPoint::UseLuckPoint(CUser* user, int itemType, int rarity, int grade)
{
    if (user == 0)
        return false;
    if (itemType != 1)
        return false;
    if (rarity != 2)
        return false;
    int rate = G_CDataManager()->m_serverParameter.getLuckPointDeductionRate(
        user->get_charac_level(), abs(grade - user->get_charac_level()));
    user->DecCurCharacLuckPoint(rate);
    return true;
}