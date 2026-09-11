void CServerEvent::TurnOffEvent(int idx)
{
    m_nCurEvent[idx] = 0;
    m_nCurEventInterval[idx] = 0;
}