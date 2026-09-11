// ---------------------------------------------------------------------------
// ORIG T 0x82eb956 (size 0x197)：ConnectP2P(CUser*, char*, int)
// ---------------------------------------------------------------------------
// [b28d-62] AE：正向 if 包裹（P257/P285）消除体尾 `jmp+nop` 中继；p167 CLEAR；
//   108=108。ORIG 0x082eb956
void CAssaultPlace::ConnectP2P(CUser* user, char* ip, int len)
{
    if (m_state != 3)
        throw (ENUM_ERROR)0x13;

    char allConnected = 1;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        if (m_assaulters[i].GetUser() == user) {
            if ((int)m_assaulters[i].GetState() != 3)
                throw (ENUM_ERROR)0x12;
            m_assaulters[i].SetState((ENUM_ASSAULT_STATE)4);
            m_assaulters[i].SetUserConnectP2P(ip, len);
        }
        if ((int)m_assaulters[i].GetState() != 4)
            allConnected = 0;
    }

    // [b28d-62] 正向包裹复现 ORIG 尾部 fall-through（早返 `if(!allConnected)return;`
    // 使 4.4 发体尾 `jmp <epilogue>; nop` 中继，ORIG 4.1 直落 leave；P257/P285）。
    if (allConnected) {
        StartAssault();
        if (m_parties[0].m_field00 != 0)
            ((CParty*)m_parties[0].m_field00)->SetAssaultState('\x04');
        if (m_parties[1].m_field00 != 0)
            ((CParty*)m_parties[1].m_field00)->SetAssaultState('\x04');
    }
}