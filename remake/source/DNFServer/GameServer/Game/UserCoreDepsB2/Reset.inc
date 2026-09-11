// ---------------------------------------------------------------------------
// 5b. Secu_MonsterCheck::Reset —— ORIG T 0x0827ac7a
// （报告 Secu_MonsterCheck/Reset.md）：各检查 word==1 且用户在册时上报
// hacktype 0xdf/0xe0/0xe1 各 1 次，随后清三个 active 字节。
// ---------------------------------------------------------------------------
void Secu_MonsterCheck::Reset()
{
    if (m_field10 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xdf, 1, 0, 0);
    if (m_field1a == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe0, 1, 0, 0);
    if (m_field26 == 1 && m_user != 0)
        m_user->getHackAnalyzer()->addServerHackCnt(
            m_user, (WongWork::ENUM_HACKTYPE)0xe1, 1, 0, 0);
    m_active223 = 0;
    m_active224 = 0;
    m_active225 = 0;
}