// ---------------------------------------------------------------------------
// ORIG T 0x82e804a (size 0x4d)：_BackupPartyInfo(CParty*, ENUM_ASSAULT_TEAM)
// this+(team*4)+0x12c —— 即 m_parties[team]（PartyInfo::Set 写指针）
// ---------------------------------------------------------------------------
void CAssaultPlace::_BackupPartyInfo(CParty* party, ENUM_ASSAULT_TEAM team)
{
    // ORIG 双路各发一次 PartyInfo::Set（0x82e806f/0x82e8090）：显式 if/else
    // 物化两 call（4.4 -O0 不合并两路 ⇒ 逐条同）。
    if (party != NULL)
        m_parties[team].Set(party);
    else
        m_parties[team].Set(NULL);
}