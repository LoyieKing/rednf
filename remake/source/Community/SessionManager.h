//
// Created by Loyie King on 2024/3/17.
//

#pragma once

#include <map>
#include <cstddef>
#include <sys/epoll.h>

#include "NetworkSession.h"
#include "PacketDispatcher.h"
#include "PacketHeader.h"

// 原始：ISessionManager 接口位于 SessionManager.cpp TU（虚表/ctor 与 CSessionManager 同 TU）。
class ISessionManager {
public:
    virtual CPacketDispatcher* GetPacketDispatcher() = 0;
    virtual bool RegisterSession(CNetworkSession* networkSession, ENUM_SESSION_TYPE sessionType) = 0;
    virtual bool UnregisterSession(CNetworkSession* networkSession, ENUM_SESSION_TYPE sessionType) = 0;
    virtual bool SendPacketToConnectingSession(char const* what1, int what2, char const* what3, int what4) = 0;
};

namespace socket_event {

// 原始：CEpoll 模板实例（CNetworkSession 特化）位于 SessionManager.cpp TU。
template<class Session>
class CEpoll {
    epoll_event *epollEvents;  // offset 4
    int epfd;                  // offset 8
    int eventCount;            // offset 0xc

public:
    // 原始反编译：构造仅清零 epollEvents/epfd（eventCount 保持未初始化）
    CEpoll() : epollEvents(NULL), epfd(0) {}
    int CreateEvents(int size);
    bool RegisterSession(Session *session, int triggerSessionEventType);
    bool UnregisterSession(Session *session);
    int WaitForEvent(int timeout);
    virtual ~CEpoll();
};

}  // namespace socket_event

namespace network_engine {

// <CNetworkSession, CEpoll> — 与原始二进制一致：第二参数为模板模板参数。
// 原始 mangled 名为 CSessionProxy<CNetworkSession, socket_event::CEpoll>（CEpoll 无实参），
// 重建原为具体类型 CEpoll<Session>，导致全部方法名不匹配。
template<class Session, template<class> class Epoll>
class CSessionProxy : Epoll<Session> {
public:
    void CreateEvents(int size);
    bool TryConnect(char const* ip, int port);
    void TryDisconnect(Session* networkSession);
    bool TryListen(const char* ip, int port);
    void WaitForSessionEvent(int timeout);
    ~CSessionProxy();
};

}  // namespace network_engine

// 原始二进制：std::map<int, stSessionInfo>（仅构造使用，未参与读写；字段以推断的最小结构对齐符号）
struct stSessionInfo {
    int value;
};

class CSessionManager : public ISessionManager, public network_engine::CSessionProxy<CNetworkSession, socket_event::CEpoll> {
    std::map<int, stSessionInfo> map;    // offset 0x14
    CNetworkSession* networkSession;     // offset 0x2c
    CPacketDispatcher packetDispatcher;  //  offset 0x30

public:
    CSessionManager();
    static bool SendHeartbeat(char const* data, int length);
    CPacketDispatcher* GetPacketDispatcher();
    bool RegisterSession(CNetworkSession* networkSession, ENUM_SESSION_TYPE sessionType);
    bool UnregisterSession(CNetworkSession* networkSession, ENUM_SESSION_TYPE sessionType);
    bool SendPacketToConnectingSession(char const* what1, int what2, char const* what3, int what4);
    ~CSessionManager();
};
