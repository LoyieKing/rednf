// ============================================================================
// CParty::setBestClearTime —— ORIG T 0x085be178（setBestClearTime.md）。
// 遍历 4 个成员槽：checkValidUser(i)==1 时 GetCharacExpandData(user, 0x12)
// 得 BestClearTime*，调 BestClearTime::setBestClearTime(dungeonIdx, diff, time)。
// ============================================================================
void CParty::setBestClearTime(int dungeonIdx, int difficulty, int time)
{
    for (int i = 0; i < 4; ++i)
    {
        // ORIG：checkValidUser 后 xor$1/test/jne（!f 裸形）；GetCharacExpandData
        // 返回值 %eax 寄存器直传（无具名局部槽，P0 右值内联）。
        if (!checkValidUser(i))
            continue;
        reinterpret_cast<BestClearTime*>(m_member[i].m_pUser->GetCharacExpandData(
            (ENUM_CHARAC_EXPAND_TYPE)0x12))
            ->setBestClearTime(dungeonIdx, difficulty, time);
    }
}