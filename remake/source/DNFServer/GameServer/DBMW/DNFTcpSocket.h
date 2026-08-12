#ifndef DBMW_DNFTCPSOCKET_H_
#define DBMW_DNFTCPSOCKET_H_

#include "DBMWCommon.h"
#include <netinet/in.h>


class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();
    bool open();
    bool connect(const char* ip, unsigned short port);
    char setOptNonBlock();
    bool bind(unsigned short port, bool flag);
    bool listen(int backlog);
    bool pollReadEvent() const;
    char pollWriteEvent() const;
    char pollErrorEvent() const;
    int pollReadWriteErrEvent() const;
    bool accept(TCPSocket& sock);
    int getHandle() const;
    int send(char* buf, int len);
    int recv(char* buf, int len);
    void close();
    int shutdown(int how);
    char* getPeerIP();
    char setOptResizeSendBuf(int size);
    char setOptResizeRecvBuf(int size);
    char setOptReuseAdrs(bool flag);
    char setOptLinger(bool flag);
    char* getPeerAdrs();
    unsigned short getPeerPort();
    int m_fd;       // +0
    struct sockaddr_in m_data4;  // +4（accept 填写的对端地址；sin_port @+6）
    char m_addr[4];     // +0x14（原版仅 4 字节地址域）
    unsigned short m_port;  // +0x18
    char m_pad1A[2];
};

#endif  // DBMW_DNFTCPSOCKET_H_
