// ORIG 08610262：参数对按升序存储（m_a <= m_b）
StatisticsNatType::NAT_TYPE_PEER_TO_PEER::NAT_TYPE_PEER_TO_PEER(int a, int b)
{
    if (a > b)
    {
        m_a = b;
        m_b = a;
    }
    else
    {
        m_a = a;
        m_b = b;
    }
}