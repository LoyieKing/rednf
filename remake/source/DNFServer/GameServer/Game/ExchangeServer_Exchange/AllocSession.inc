// ---------------------------------------------------------------------------
// AllocSession / FreeSession               ORIG 0x849104a / 0x8491060
// （StaticPool<CSession,300>@+0x6c）
// ---------------------------------------------------------------------------
CSession* CExchangeServer::AllocSession()
{
    return m_sessionPool.Acquire();
}