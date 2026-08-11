#ifndef SOCKET_H_
#define SOCKET_H_

#include <queue>
#include <set>
#include <netinet/in.h>

#include "ThreadLock.h"

// TCPSocket：sock_@0 / adrs_@4[16] / peer_ip@0x14 / peer_port@0x18，总 0x1c（无显式 pad）
class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();
    bool open();
    bool bind(unsigned short port, bool bNonBlock);
    bool listen(int backlog);
    ssize_t send(char* buf, int len);
    ssize_t recv(char* buf, int len);
    int getHandle() const
    {
        return sock_;
    }
    int shutdown(int how);
    void close();
    bool setOptNonBlock();
    bool setOptReuseAdrs(bool bReuse);
    bool setOptLinger(bool bLinger);
    bool setOptNagle(bool bNagle);
    bool setOptRecvBufferSize(int size);
    bool connect(char const* ip, unsigned short port);
    bool pollReadEvent() const;
    bool pollWriteEvent() const;
    bool pollErrorEvent() const;
    bool accept(TCPSocket& new_sock);
    char* getPeerAdrs();
    unsigned short getPeerPort();
    int setOptResizeSendBuf(int size);
    int setOptResizeRecvBuf(int size);

private:
    int sock_;                  // +0
    sockaddr_in adrs_;          // +4 (16B)
    unsigned int peer_ip_;      // +0x14
    unsigned short peer_port_;  // +0x18
};

// UDPSocket：sock_@0 / adrs_@4[16] / port_@0x14 / from_@0x18[16] /
//           monitor_queue@0x28 / monitor_set@0x50 / lock@0x68，总 0x80
class UDPSocket
{
public:
    UDPSocket();
    ~UDPSocket();
    bool open();
    bool bind(unsigned short port, bool bNonBlock);
    bool setOptNonBlock();
    void delDisconnectUser(unsigned int acc_id);
    void pushMonitorAuthPacket(unsigned int acc_id);
    void popMonitorAuthPacket();
    unsigned int sizeMonitorAuthPacket();
    ssize_t send(char* buf, int len, unsigned short port, const char* ip, unsigned int ip_addr);
    ssize_t recv(char* buf, int len);
    int getHandle();
    void close();
    int setOptResizeSendBuf(int size);
    int setOptResizeRecvBuf(int size);
    bool pollReadEvent() const;
    bool pollWriteEvent() const;
    bool pollErrorEvent() const;

private:
    int sock_;
    sockaddr_in adrs_;
    unsigned short port_;
    sockaddr_in from_;
    std::queue<unsigned int> monitor_queue;
    std::set<unsigned int> monitor_set;
    TThreadLock<ThreadLock_linux> lock_;
};

#endif // SOCKET_H_
