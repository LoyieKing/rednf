#ifndef SOCKET_H_
#define SOCKET_H_

#include "stdafx.h"
#include <netinet/in.h>

class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();
    bool open();
    bool bind(unsigned short port, bool bNonBlock);
    bool listen(int back_log);
    bool connect(const char* ip, unsigned short port);
    int send(char* buf, int size);
    int recv(char* buf, int size);
    void close();
    int shutdown(int how);
    bool accept(TCPSocket& accepted);
    bool setOptLinger(bool b);
    bool setOptNonBlock();
    bool setOptReuseAdrs(bool b);
    bool setOptResizeSendBuf(int size);
    bool setOptResizeRecvBuf(int size);
    SOCKET getHandle() const;
    unsigned char* getPeerAdrs();
    unsigned short getPeerPort();
    bool pollReadEvent() const;
    bool pollWriteEvent() const;
    bool pollErrorEvent() const;

private:
    int sock_;
    sockaddr_in adrs_;
    unsigned char c_adrs_[4];
    unsigned short port_;
};

class UDPSocket
{
public:
    UDPSocket();
    ~UDPSocket();
    bool open();
    bool bind(unsigned short port, bool bNonBlock);
    int send(char* buf, int size, unsigned short nPort, const char* szDestIp);
    int recv(char* buf, int size);
    void close();
    bool setOptNonBlock();
    bool setOptResizeSendBuf(int size);
    bool setOptResizeRecvBuf(int size);
    SOCKET getHandle();
    bool pollReadEvent() const;
    bool pollWriteEvent() const;
    bool pollErrorEvent() const;

private:
    int sock_;
    sockaddr_in adrs_;
    unsigned short port_;
};

#endif // SOCKET_H_
