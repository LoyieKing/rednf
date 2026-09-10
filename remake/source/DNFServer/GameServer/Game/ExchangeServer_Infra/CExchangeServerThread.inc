CExchangeServerThread::CExchangeServerThread()
{
    CSession::exchange_server_ = static_cast<IExchangeServer*>(&m_exchangeServer);
