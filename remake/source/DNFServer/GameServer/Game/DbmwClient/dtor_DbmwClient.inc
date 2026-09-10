DbmwClient::~DbmwClient()
{
    m_connected = false;
    close(m_sock);
    m_initialized = false;
    m_sock = -1;
    // ORIG 0x081204f3：`call _ZdlPv`（operator delete(void*)，非 delete[]）且
    // 调用前**无 test/je 判空**（4.4 对 delete[] 自带判空，多 3 条）⇒ 显式
    // operator delete 调用复现 4.1 直落形（P207）。
    ::operator delete(m_recvBuf);
