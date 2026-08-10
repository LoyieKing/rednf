#ifndef DNFTCPSOCKET_H_
#define DNFTCPSOCKET_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class TCPSocket;

// from GuildDomain.h
class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();
    bool open();
    int bind(unsigned short port, bool flag);
    bool listen(int backlog);
    int send(char* buf, int len);
    int recv(char* buf, int len);
    bool pollReadEvent() const;
    bool pollWriteEvent() const;
    bool pollErrorEvent() const;
    int pollReadWriteErrEvent() const;
    int getHandle() const;
    int shutdown(int how);
    void close();
    bool setOptNonBlock();
    bool setOptReuseAdrs(bool flag);
    bool setOptLinger(bool flag);
    bool connect(const char* ip, unsigned short port);
    int accept(TCPSocket& peer);
    char* getPeerIP();
    char* getPeerAdrs();
    unsigned short getPeerPort();
    int setOptResizeSendBuf(int size);
    int setOptResizeRecvBuf(int size);
    int m_sock;      // +0
    char m_data[0x18];   // +4：sockaddr(0x10) + peerAdrs(+0x14) + peerPort(+0x18)
};

#endif
