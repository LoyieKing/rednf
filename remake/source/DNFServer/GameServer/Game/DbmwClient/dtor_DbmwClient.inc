// ORIG T _ZN10DbmwClientD1Ev/D2Ev @0x081204be（0x3c）：
// m_connected(+0x02)=0 → close(m_sock(+0x08)) → m_initialized(+0x01)=0 → m_sock=-1
// → operator delete(m_recvBuf(+0x2c))。【Main 补定义：ORIG 4.1 -O0 无条件发射，历轮误删】
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
}