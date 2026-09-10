void CExchangeServer::SetMaster()
{
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0xf6, 0);
    trace("SetMaster()");
    SetServerType(EXCHANGE_SERVER_MASTER);
    m_flag38 = true;
