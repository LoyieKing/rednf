void CExchangeServer::SetSlave()
{
    cMyTrace trace(
        __PRETTY_FUNCTION__, 0xfe, 0);
    trace("SetSlave()");
    SetServerType(EXCHANGE_SERVER_SLAVE);
    m_flag38 = false;
