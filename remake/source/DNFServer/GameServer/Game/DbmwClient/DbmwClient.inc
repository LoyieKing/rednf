// ORIG @0x08120454：+0=+1=+2=0、-4=0、+8=-1、new(0x200)、
// +0x1038=0、memset(+0x34,0,0x1001)。
DbmwClient::DbmwClient()
    : m_initialized(false),
      m_connected(false),
      m_sock(-1),
      m_recvBuf(new char[0x200]),
      m_queryLen(0)
{
    memset(m_query, 0, sizeof(m_query));
    m_reconnectCount = 0;
}