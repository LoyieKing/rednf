void CParty::historyBloodRoundClear(int round, unsigned int clearTime,
                                    unsigned int playTime, const char* dungeonName)
{
    // [b28d-solo-CPartyDepsD2] P116/P253：ORIG 0x085b7dce rep stos 的 %edx=0x40 是
    // dword 计数 ⇒ 缓冲实为 0x100 字节（旧 char[0x40] 少清 0xc0 字节且溢出）。
    // P0：cUserHistoryLog 指针 ORIG 无槽往返（lea 0x79700(%eax),%edx 直压参）
    // ⇒ 内联成员访问，删具名 log 局部。
    char memberNames[0x100] = { '\0' };
    getMemberNames(memberNames);
    for (int slot = 0; slot < 4; ++slot)
    {
        if (get_user(slot) == 0 || !checkValidUser(slot))
            continue;
        m_member[slot].m_pUser->m_historyLog.BloodRoundClear(
            round, clearTime, playTime, dungeonName, memberNames);
    }
