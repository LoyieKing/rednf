bool CExchangeServer::GetNetworkEvent()
{
    m_sessionMgr.WaitForSessionEvent(10);
    return true;
