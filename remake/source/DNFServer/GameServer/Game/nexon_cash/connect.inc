// ORIG 0x081af9ea：本地 sockaddr_in{AF_INET,inet_addr(host),htons(port)}；
// connect 失败→false；成功则 memcpy(&m_peerAdrs,&sin_addr,4)、
// m_peerPort=sin_port，return true。
// [b28d-D43] P75 实参装载时刻：ORIG 提前 `mov -0xc(%ebp),%ecx`（跨两条 this 装载）vs 4.4 使用点才读（ORIG 0x081af9ea）。
bool TCPSocket::connect(const char* host, unsigned short port)
