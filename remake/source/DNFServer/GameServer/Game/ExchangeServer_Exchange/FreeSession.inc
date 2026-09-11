void CExchangeServer::FreeSession(CSession* session)
{
    m_sessionPool.Free(session);
}