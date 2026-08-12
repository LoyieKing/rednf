#ifndef DNF_UDP_HANDLER_H_
#define DNF_UDP_HANDLER_H_

// ---- CUdpHandler：0x10 ----
class CUdpHandler
{
public:
    CUdpHandler();
    ~CUdpHandler();
    unsigned int InetAddr(const char* ip) const;
    int InitServerSocket(int port);
    int InitClientSocket();
    // ORIG 返回类型 bool：调用方 !RecvFromClient(...) 编译为 xor $1。
    bool RecvFromClient(char* buf, int* size, unsigned int* addr, unsigned short* port) const;
    bool SendToClient(char* buf, int len, unsigned short port, const char* ip,
                      unsigned int addr) const;
    int SendToServer(char* buf, int len, unsigned short port, const char* ip) const;
    char RecvFromServer(char* buf, int* size, unsigned int* addr, unsigned short* port) const;
    int GetServerSocket();
    int m_sock;         // +0
    int m_clientSock;   // +4
};

#endif  // DNF_UDP_HANDLER_H_
