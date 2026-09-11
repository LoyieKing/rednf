// ORIG T 0x085b357c（GetCoinCount.md）：INT_MAX - 该成员 BattleData 已用硬币。
int CParty::GetCoinCount(CUser* user)
{
    for (int i = 0; i <= 3; i++)
    {
        if (!_checkValidUser(i))
        {
            continue;
        }
        if (m_member[i].m_pUser == user)
        {
            // [P49] 弃 static inline helper（产真 call）：直成员访问让
            // 4.4 折叠 scale4 单段（ORIG `add $0x2c0,%edx; mov 0x14(%eax,%edx,4)`）。
            return 0x7fffffff - m_battleData.m_arr7e8[i];
        }
    }
    return 0;
}