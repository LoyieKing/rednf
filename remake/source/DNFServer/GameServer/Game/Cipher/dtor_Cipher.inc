Cipher::~Cipher()
{
    for (int i = 0; i <= 0xd; ++i)
    {
        DestroyCryptoGraph(m_graphs[i]);
        m_graphs[i] = 0;
    }
    m_graphs.clear();
