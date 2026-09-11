int CServerEvent::SubExpRate(int rate)
{
    m_nExpRate -= rate;
    return m_nExpRate;
}