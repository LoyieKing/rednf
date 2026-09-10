void DbmwClient::SetServerAddress(const char* ip, unsigned short port)
{
    // ORIG @0x08120af6。
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip);
    m_addr.sin_port = htons(port);
