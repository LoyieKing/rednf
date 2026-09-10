void CServerEvent::TurnOnEvent(int idx, int durationSecond)
{
    m_nCurEvent[idx] = 1;
    m_nCurEventInterval[idx] = durationSecond;
