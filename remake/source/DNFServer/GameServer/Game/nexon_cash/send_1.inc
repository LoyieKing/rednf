// ORIG 0x081af000：
//   if (!data || length<=0) return -1; if (!host) return 0;
//   目标 sockaddr_in{AF_INET,htons(port),inet_addr(host)}；
//   n = sendto(m_socket,data,length,0,&adrs,16)；
//   n<0 且 errno∈{EWOULDBLOCK,EAGAIN,EINTR}→0；否则 n。
int UDPSocket::send(char* data, int length, unsigned short port,
