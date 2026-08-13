#ifndef NSL_TCPSOCKET_H_
#define NSL_TCPSOCKET_H_

#include <netinet/in.h>

namespace nsl {

typedef int SOCKET;

class TCPSocket
{
public:
    TCPSocket();
    ~TCPSocket();

    bool open();
    void shutdown(int opt);
    void close();
    bool bind(unsigned short port, bool bNonBlock);
    bool listen(int backlog);
    int send(char* buf, int len);
    int recv(char* buf, int len);
    bool connect(const char* ip, unsigned short port);
    bool connect_nonb(const char* ip, unsigned short port, timeval tval);
    bool accept(TCPSocket& accepted);
    int pollReadEvent() const;
    int pollWriteEvent() const;
    int pollErrorEvent() const;
    int setOptNonBlock();
    bool setOptReuseAdrs(bool b);
    bool setOptLinger(bool b);
    bool setOptNagle(bool b);
    bool setOptResizeSendBuf(int size);
    bool setOptResizeRecvBuf(int size);
    static void SetRecvBufSize(int size);
    static void SetSendBufSize(int size);
    SOCKET getHandle() const;
    unsigned char* getPeerAdrs();
    unsigned short getPeerPort();

    SOCKET sock_;
    sockaddr_in adrs_;
    unsigned char c_adrs_[4];
    unsigned short port_;

    static int msRecvBufSize;
    static int msSendBufSize;
    static const int MAX_SEND_RETRY;
    int mSendRetryCount;
};

class UDPSocket
{
public:
    UDPSocket();
    ~UDPSocket();
    bool open();
    bool bind(const char* strIp, unsigned short port);
    bool bind(unsigned short port, bool bNonBlock);
    bool setOptNonBlock();
    int send(char* buf, const int size, unsigned short nPort, const char* szDestIp);
    int recv(char* buf, const int size);
    SOCKET getHandle();
    void close();
    bool setOptResizeSendBuf(int size);
    bool setOptResizeRecvBuf(int size);
    int pollReadEvent() const;
    int pollWriteEvent() const;
    int pollErrorEvent() const;

    SOCKET sock_;
    sockaddr_in adrs_;
    unsigned short port_;
    sockaddr_in from_;
};

} // namespace nsl

#endif // NSL_TCPSOCKET_H_
