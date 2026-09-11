// ORIG 0x081aedc6：保存主机序端口；清零 m_adrs；
// m_adrs={AF_INET=2, inet_addr(host)}；sin_port=htons(m_port)；
// bind(m_socket,&m_adrs,16)；失败→printf("Udp Port binding fail, %s , %d\n",
// host, port)（@0x08bb4a08）并 false。
// [b28d-D43] AE: m_adrs 具名 sockaddr_in 视图直存 + htons(m_port)（ORIG 0x081aedc6）。
bool UDPSocket::bind(const char* host, unsigned short port)
