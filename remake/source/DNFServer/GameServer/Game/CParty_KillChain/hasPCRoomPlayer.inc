// ============================================================================
// hasPCRoomPlayer —— ORIG T 0x085b4a70。
// 任一有效成员持有 0xc 号 premium → true。
// ============================================================================
bool CParty::hasPCRoomPlayer()
{
    for (int i = 0; i <= 3; ++i)
    {
        // ORIG 0x85b4a7f 域：两独立守卫（xor;test;jne continue → call;test;je
        // skip）——合并 && 违 P133（汇聚 setne 形）。
        if (!_checkValidUser(i))
            continue;
        if (m_member[i].m_pUser->isAffectedPremium(
                static_cast<ENUM_PREMIUM_TYPE>(0xc)))
            return true;
    }
    return false;
}