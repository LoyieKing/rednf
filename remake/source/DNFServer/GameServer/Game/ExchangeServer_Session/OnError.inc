bool CSession::OnError()
{
    cMyTrace(__PRETTY_FUNCTION__, 0x1ec, 0)("ExchangeServer OnError() Socket[%d] Error(%s)",
        GetSocket(), strerror(errno));
    return false;
