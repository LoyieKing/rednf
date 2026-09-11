// ORIG 0x081af0de：
//   if (!buffer || length<=0) return -1;
//   addrlen=16；n=recvfrom(m_socket,buffer,length,0,(sockaddr*)&m_from,&addrlen)
//   —— ORIG 直接以 this+0x18 作为源地址存储（即成员 m_from）。
//   n<0: (errno==EAGAIN||errno==EINTR||errno==EAGAIN)→ return 0（0x081af162 共岛），
//        否则 → return -1（0x081af169）；n>=0 原样返回（0x081af170）。
// [b28d-D43] P75/P52：ORIG 提前 `lea -0x10(%ebp),%ebx`（callee-saved 独占）vs 4.4 推迟到实参装载点（ORIG 0x081af0de）。
int UDPSocket::recv(char* buffer, int length)
