// ---------------------------------------------------------------------------
// GetServerType / SetServerType            ORIG 0x8491030 / 0x849103c
// ---------------------------------------------------------------------------
EXCHANGE_SERVER_TYPE CExchangeServer::GetServerType()
{
    return (EXCHANGE_SERVER_TYPE)m_serverType68;                    // +0x68
}