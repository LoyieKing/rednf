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
    bool bind(unsigned short port, bool flag);
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
    bool accept(TCPSocket& peer);
    char* getPeerIP();
    char* getPeerAdrs();
    unsigned short getPeerPort();
    int setOptResizeSendBuf(int size);
    int setOptResizeRecvBuf(int size);
    int m_sock;      // +0
    union {
        char m_sockaddr[0x10];     // +4
        struct {
            unsigned short m_sin_family; // +4
            unsigned short m_sin_port;   // +6
            char m_sin_addr[4];          // +8
            char m_sin_zero[8];          // +0xc
        };
    };
    char m_peerAdrs[4];     // +0x14
    unsigned short m_peerPort; // +0x18
    char m_pad[2];          // +0x1a
};

#endif
