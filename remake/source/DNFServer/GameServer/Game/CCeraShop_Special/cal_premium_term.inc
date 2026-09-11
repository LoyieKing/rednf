// ============================================================================
// premium_helper_function（免费命名空间）—— ORIG 0x827cff0
// ============================================================================
// if(flag) term *= (byte)CDataManager+0x877e；return term * extra。
int premium_helper_function::cal_premium_term(int term, bool flag, int extra)
{
    if (flag)
        return (int)G_CDataManager()->m_boosterGage.m_cMultiboxCount * term * extra;
    return term * extra;
}