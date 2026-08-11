//
// Created by loyieking on 24-3-23.
//

#include "SessionManager.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <sys/epoll.h>

#include <cerrno>
#include <cstring>

#include "ArchiveLog.h"
#include "NetworkSession.h"
#include "UserManager.h"

CSessionManager::CSessionManager() {
    CNetworkSession::pSessionManager = this;
    networkSession = NULL;
    // 来自反编译/DWARF 的推断：原始条件为 size <= i 时 break，即遍历 0..size-1
    // 原重建代码的 "size > i break" 逻辑完全反转，导致一次都不执行注册
    for (int i = 0; i < get_packet_table_size(); i++) {
        packetDispatcher.register_packet(get_packet_table()[i].packet_id, get_packet_table()[i].packet_handler);
    }
}
CPacketDispatcher* CSessionManager::GetPacketDispatcher() {
    return &packetDispatcher;
}

bool CSessionManager::RegisterSession(CNetworkSession* registeringSession, ENUM_SESSION_TYPE sessionType) {
    if (sessionType == SESSION_TYPE_SERVER) {
        this->networkSession = registeringSession;
    }
    return true;
}
bool CSessionManager::UnregisterSession(CNetworkSession* unregisteringSession, ENUM_SESSION_TYPE sessionType) {
    if (sessionType == SESSION_TYPE_SERVER) {
        this->networkSession = NULL;
    } else if (sessionType == SESSION_TYPE_USER) {
        g_user_manager.leave_user(unregisteringSession);
    }
    return true;
}
bool CSessionManager::SendHeartbeat(const char* data, int length) {
    return true;
}

bool CSessionManager::SendPacketToConnectingSession(char const* what1, int what2, char const* what3, int what4) {
    // 原始：networkSession != NULL 外层；Send == false 提前处理（xor eax,1; test/je）
    if (networkSession != NULL) {
        if (networkSession->Send(what1, what2) == false) {
            if (what3 != NULL && what4 != 0) {
                TryDisconnect(networkSession);
                TryConnect(what3, what4);
            }
            return false;
        }
        return true;
    } else {
        if (what3 != NULL && what4 != 0) {
            TryConnect(what3, what4);
        }
        return false;
    }
}
CSessionManager::~CSessionManager() {
}

// ===== socket_event::CEpoll（原始位于 SessionManager.cpp TU）=====

namespace socket_event {

template<class What>
inline int CEpoll<What>::CreateEvents(int size) {
    this->epfd = epoll_create(size);
    if (this->epfd < 0) {
        ArchiveLog("[Epoll::init] Can\'t init epoll create\n");
        return 0;
    } else {
        // pvVar3 = operator.new[](param_1 * 0xc);
        this->epollEvents = new epoll_event[size];
        if (this->epollEvents == NULL) {
            ArchiveLog("[Epoll::init] Can\'t alloc event memory\n");
            return 0;
        } else {
            this->eventCount = size;
            return 1;
        }
    }
}

template<class Session>
bool CEpoll<Session>::RegisterSession(Session *session, int triggerSessionEventType) {
    (session)->SetTriggerSessionEventType(triggerSessionEventType);
    epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLHUP;
    event.data.ptr = session;
    // 原始：call epoll_ctl 后 shr eax,0x1f + test al,al + je（调用结果直接入条件 < 0）
    // GetSocket() 结果直接作为实参（mov [esp+8],eax，不溢栈）
    if (epoll_ctl(this->epfd, EPOLL_CTL_ADD /*1*/, (session)->GetSocket(), &event) < 0) {
        return 0;
    }
    return 1;
}

template<class Session>
bool CEpoll<Session>::UnregisterSession(Session *session) {
    epoll_event event;
    memset(&event, 0, sizeof(event));
    // 原始：call epoll_ctl 后 shr eax,0x1f + test al,al + je（调用结果直接入条件 < 0）
    if (epoll_ctl(this->epfd, EPOLL_CTL_DEL /*2*/, (session)->GetSocket(), &event) < 0) {
        return 0;
    }
    return 1;
}

template<class Session>
int CEpoll<Session>::WaitForEvent(int timeout) {
    int count = epoll_wait(this->epfd, this->epollEvents, this->eventCount, timeout);
    if (count < 0 && errno != EINTR /*4*/ && errno != 0) {
        return 0;
    }
    // 原始：succeed 在循环外初始化（mov BYTE [ebp-0x1d],1），循环体内再显式赋值
    bool succeed = true;
    for (int i = 0; i < count; i++) {
        Session *session = (Session *)this->epollEvents[i].data.ptr;
        // 原始：newSession 先初始化 0（mov DWORD [ebp-0x14],0）再 succeed=true
        Session *newSession = NULL;
        succeed = true;
        if (this->epollEvents[i].events & EPOLLIN) {
            // 原始：(type & 8) != 0 分支在前（OnAccept），== 0 为 else（OnRecv）
            if (((session)->GetTriggerSessionEventType() & 8) != 0) {
                // 原始：OnAccept 结果经独立临时承接（mov DWORD [ebp-0x10],0 再赋值）
                Session *ns = NULL;
                ns = session->OnAccept();
                if (ns != NULL) {
                    RegisterSession(ns, 7);
                }
            } else {
                if ((session)->GetTriggerSessionEventType() & 1) {
                    succeed = session->OnRecv();
                }
            }
        }
        if (this->epollEvents[i].events & EPOLLOUT) {
            // 原始：(type & 0x10) != 0 分支在前（OnConnect），== 0 为 else（OnSend）
            if (((session)->GetTriggerSessionEventType() & 0x10) != 0) {
                succeed = (session)->OnConnect();
                if (succeed) {
                    (session)->SetTriggerSessionEventType(7);
                }
            } else {
                if ((session)->GetTriggerSessionEventType() & 2) {
                    int size = (session)->OnSend();
                    if (size < 0) {
                        succeed = false;
                    }
                }
            }
        }
        if (this->epollEvents[i].events & (EPOLLERR | EPOLLHUP)) {
            succeed = (char)(session)->OnError();
        }
        if (!succeed) {
            UnregisterSession(session);
            (session)->Disconnect();
        }
    }
    return 1;
}

template<class Session>
CEpoll<Session>::~CEpoll() {
    if (this->epollEvents != NULL) {
        delete[] this->epollEvents;
        this->epollEvents = NULL;
    }
}

template class CEpoll<CNetworkSession>;

}  // namespace socket_event

// ===== network_engine::CSessionProxy（原始位于 SessionManager.cpp TU）=====

namespace network_engine {

template<class Session, template<class> class Epoll>
void CSessionProxy<Session, Epoll>::CreateEvents(int size) {
    Epoll<Session>::CreateEvents(size);
}

template<class Session, template<class> class Epoll>
bool CSessionProxy<Session, Epoll>::TryConnect(char const* ip, int port) {
    CNetworkSession* session = CNetworkSession::Connect(ip, port);
    if (session == NULL) {
        return false;
    }
    if (!Epoll<Session>::RegisterSession(session, /*0x14*/ EPOLLHUP | EPOLLOUT)) {
        return false;
    }
    session->AddTriggerSessionEventType(/*0x10*/ 0x10);
    return true;
}

template<class Session, template<class> class Epoll>
void CSessionProxy<Session, Epoll>::TryDisconnect(Session* networkSession) {
    Epoll<Session>::UnregisterSession(networkSession);
    networkSession->Disconnect();
}

template<class Session, template<class> class Epoll>
bool CSessionProxy<Session, Epoll>::TryListen(const char* ip, int port) {
    // 原始：先初始化 NULL（mov [ebp-0xc],0），赋值在条件内（cmp; sete al; test al,al; je）
    CNetworkSession* session = NULL;
    if ((session = CNetworkSession::CreateListenSocket(port, ip)) == NULL) {
        return 0;
    }
    return Epoll<Session>::RegisterSession(session, /*0x8*/ EPOLLERR);
}

template<class Session, template<class> class Epoll>
void CSessionProxy<Session, Epoll>::WaitForSessionEvent(int timeout) {
    Epoll<Session>::WaitForEvent(timeout);
}

template<class Session, template<class> class Epoll>
CSessionProxy<Session, Epoll>::~CSessionProxy() {}

template class CSessionProxy<CNetworkSession, socket_event::CEpoll>;

}  // namespace network_engine
