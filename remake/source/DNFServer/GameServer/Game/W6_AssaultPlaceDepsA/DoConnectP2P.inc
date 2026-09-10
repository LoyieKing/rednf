void CAssaultPlace::DoConnectP2P()
{
    m_state = (ENUM_ASSAULT_STATE)3;
    for (int i = 0; i < 8; ++i) {
        if (m_assaulters[i].Empty())
            continue;
        m_assaulters[i].SetState((ENUM_ASSAULT_STATE)3);
    }
    TimerConnectP2PAssaultTimeout::registNextTimer(
        0xf, m_field104 /*+0x104*/, m_authenValue /*+0x108*/);
