// ---------------------------------------------------------------------------
// ORIG T 0x82e82a2 (size 0x5d)：_SetReadyState() —— 场状态置 2 + 全员 state 1
// ---------------------------------------------------------------------------
void CAssaultPlace::_SetReadyState()
{
    m_state = (ENUM_ASSAULT_STATE)2;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        m_assaulters[i].SetState((ENUM_ASSAULT_STATE)1);
    }
}