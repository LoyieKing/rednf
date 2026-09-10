CSession* CExchangeServer::AllocSession()
{
    return m_sessionPool.Acquire();
