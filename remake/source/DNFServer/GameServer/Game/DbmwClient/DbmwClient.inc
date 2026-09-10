DbmwClient::DbmwClient()
    : m_initialized(false),
      m_connected(false),
      m_sock(-1),
      m_recvBuf(new char[0x200]),
      m_queryLen(0)
{
    memset(m_query, 0, sizeof(m_query));
    m_reconnectCount = 0;
