#ifndef DNF_UDP_HANDLER_H_
#define DNF_UDP_HANDLER_H_

// CUdpHandler：sock@0 / clientSock@4
class CUdpHandler
{
public:
    CUdpHandler();
    int InitServerSocket(int port);
    int InitClientSocket();
    int RecvFromClient(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int SendToClient(char* buf, int len, unsigned short port, char const* ip,
                     unsigned int ipaddr) const;
    int RecvFromServer(char* buf, int* len, unsigned int* ip, unsigned short* port) const;
    int SendToServer(char* buf, int len, unsigned short port, char const* ip) const;
    void InetAddr(char const* ip) const;
    int m_sock;        // +0
    int m_clientSock;  // +4
};

bool SetNonBlock(int fd);
int getErrno();

#endif // DNF_UDP_HANDLER_H_
