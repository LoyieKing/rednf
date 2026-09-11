// ---------------------------------------------------------------------------
// GetNetworkEvent                          ORIG 0x84910a6
// （WaitForSessionEvent(10ms)；返回值忽略，恒 true）
// ---------------------------------------------------------------------------
bool CExchangeServer::GetNetworkEvent()
{
    m_sessionMgr.WaitForSessionEvent(10);
    return true;
}