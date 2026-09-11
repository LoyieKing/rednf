bool DbmwClient::Reconnect()
{
    // ORIG @0x08121156。
    ++m_reconnectCount;
    if (Connect() != 0)
    {
        LogManager::logFormat(
            1, "localchina/DbmwClient.cpp", __FUNCTION__, 0x2cd,
            "AccountDBMW Reconnect() Error!! Query(%s)", m_query);
        return false;
    }
    return true;
}