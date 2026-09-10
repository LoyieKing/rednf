void CAssaultPlace::_SetReadyState()
{
    m_state = (ENUM_ASSAULT_STATE)2;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        m_assaulters[i].SetState((ENUM_ASSAULT_STATE)1);
    }
