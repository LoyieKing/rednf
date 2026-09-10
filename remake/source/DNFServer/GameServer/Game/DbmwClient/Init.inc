unsigned int DbmwClient::Init()
{
    // ORIG @0x081205e6：未初始化时 socket(AF_INET=2, SOCK_STREAM=1, 0)；
    // 成功后清 +0x0c/+0x10、置 +0x01=1；sock 失败返回 0xffffff9c。
    if (m_initialized)
        return 0;

    m_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sock == (int)0xffffffff)
        return (unsigned int)-100;

    m_field0c[0] = 0;
    m_field0c[1] = 0;
    m_initialized = true;
    return 0;
