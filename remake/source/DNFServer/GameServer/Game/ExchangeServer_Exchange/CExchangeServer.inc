CExchangeServer::CExchangeServer()
    : IExchangeServer(),
      m_sessionMgr(),
      m_localInfo(),
      m_serverMap(),
      m_sessionPool()
{
    Reset();
