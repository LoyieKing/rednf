//
// Created by Loyie King on 2024/3/10.
//

#pragma once

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "PacketHeader.h"
#include "ReverseEngineerLib.h"

enum ENUM_SESSION_TYPE {
    SESSION_TYPE_USER = 1,
    SESSION_TYPE_SERVER = 2,
};

class ISessionManager;
class CPacketDispatcher;

// 原始：CAbstractSocket 模板（NetworkSession 的基类）位于 NetworkSession.cpp TU。
template<int MaxRecvBuf, int MaxSendBuf>
class CAbstractSocket {
    int socket;  // offset 0
    int what1;   // offset 4

    int m_remain_recvlen;         // offset 8
    char *recvBufferOffset;        // offset 0xc
    char recvBuffer[MaxRecvBuf];  // offset 0x10

    int remainSendLen;            // offset 0x1010
    char *sendBufferOffset;        // offset 0x1014
    char sendBuffer[MaxSendBuf];  // offset 0x1018

    int triggerSessionEventType;  // offset 0x2018
    sockaddr_in socket_address;   // offset 0x201c（原始为命名字段布局：sin_family/sin_port/sin_addr）

public:
    int AcceptSocket();

    void AddTriggerSessionEventType(int sessionEventType);

    CAbstractSocket();
    ~CAbstractSocket();

    bool ConnectPeer();

    bool CreateConnectionSocket(const char *ip, int port);

    static bool CreateListenSocket(int &outputSocket, int port, const char *ip);  // 原始：bool 返回

    void disconnect();

    char *GetRecvBuff(int getSize, int &remainRecvLen);

    int GetSocket();

    int GetTriggerSessionEventType();

    ssize_t recv_packet();

    void Reset();

    int send_packet();

    int send_packet(const char *data, int last);

    static bool SetNonblock(int sock);

    bool SetRecvBufferOffset(int parsingLen);

    int SetSocket(int sock, bool setNonBlock);

    void SetTriggerSessionEventType(int sessionEventType);
};

TEST_CLASS_SIZE(sockaddr, 0x10)
typedef char _CAbstractSocket_size_check[sizeof(CAbstractSocket<4096, 4096>) == 0x202c ? 1 : -1];

class CNetworkSession : public CAbstractSocket<4096, 4096> {
    ENUM_SESSION_TYPE type;

public:
    static ISessionManager* pSessionManager;

    CNetworkSession();
    ~CNetworkSession();
    bool OnRecv();
    CNetworkSession* OnAccept();
    bool OnConnect();
    int OnSend();
    bool OnError();
    bool Disconnect();
    int Parsing(char const* data, int len);

    bool dispatch(PacketHeader* packet);
    ENUM_SESSION_TYPE get_type();
    void set_type(ENUM_SESSION_TYPE t);
    bool Send(char const* data, int len);

    static CNetworkSession* Connect(char const* ip, int port);
    static CNetworkSession* CreateListenSocket(int port, char const* ip);

    void* operator new(size_t size);
    void operator delete(void* p, unsigned int size);
};



TEST_CLASS_SIZE(CNetworkSession, 0x2030)
