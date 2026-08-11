//
// Created by Loyie King on 2024/3/10.
//

#include "NetworkSession.h"

#include <iostream>
#include "DNFFunctionLib.h"
#include "CommonConstants.h"
#include "GuildConstants.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ArchiveLog.h"
#include <boost/pool/object_pool.hpp>
#include "PacketDispatcher.h"
#include "SessionManager.h"

// 原始二进制：全局对象池 g_network_session_pool（bss 0x80df098），
// object_pool 构造参数 (next_size=0x20, max_size=0) → pool(sizeof(CNetworkSession), 0x20, 0)。
boost::object_pool<CNetworkSession, boost::default_user_allocator_new_delete> g_network_session_pool(0x20, 0);

ISessionManager *CNetworkSession::pSessionManager = NULL;

CNetworkSession::CNetworkSession()
    : CAbstractSocket<4096, 4096>() {
}

bool CNetworkSession::OnRecv() {
    int size = recv_packet();
    // 原始：主路径在前（jg 越过尾部 size 检查），size<=0 的判定在函数尾部
    if (size > 0) {
        int got = 0;
        char* buff = GetRecvBuff(size, got);
        int parsingLen = Parsing(buff, got);
        // 原始：SetRecvBufferOffset == false 提前返回（xor eax,1; test/je 形态）
        if (SetRecvBufferOffset(parsingLen) == false) {
            return 0;
        }
        return 1;
    }
    if (size < 0) {
        return false;
    }
    return true;
}
CNetworkSession *CNetworkSession::OnAccept() {
    int sock = this->AcceptSocket();
    CNetworkSession* networkSession = new CNetworkSession();
    // 原始：!= NULL 分支在前（SetSocket 先于 ArchiveLog 块）
    if (networkSession != NULL) {
        networkSession->SetSocket(sock, false);
    } else {
        ArchiveLog("session_pool_.Acquire() error");
    }
    networkSession->set_type(SESSION_TYPE_USER);
    pSessionManager->RegisterSession(networkSession, SESSION_TYPE_USER);  // (**(code **)(*pSessionManager + 4))(pSessionManager,local_20,1);
    return networkSession;
}
bool CNetworkSession::OnConnect() {
    // 原始：GetSocket() 结果直接作为变参参数（mov [esp+8],eax，无临时变量溢栈）
    ArchiveLog("OnConnect sock(%d)", GetSocket());
    return true;
}
int CNetworkSession::OnSend() {
    // 原始：send_packet() 结果直接入条件（shr eax,0x1f + test/je）
    if (send_packet() < 0) {
        ArchiveLog("OnSend() Socket[%d] Error(%s)", GetSocket(), strerror(errno));
        return -1;
    }
    return 0;
}
bool CNetworkSession::OnError() {
    ArchiveLog("OnError() Socket[%d] Error(%s)", GetSocket(), strerror(errno));
    return false;
}
bool CNetworkSession::Disconnect() {
    pSessionManager->UnregisterSession(this, type);  //(**(code **)(*pSessionManager + 8))(pSessionManager,param_1,param_1[0x80b]);
    disconnect();
    delete this;
    return true;
}
int CNetworkSession::Parsing(const char *data, int len) {
    // 原始：while (len > i) + 两个 break（setg 物化循环条件；len-i 先求值）
    ushort *p;
    ushort *p2;  // 原始：p2 = p 副本（mov [ebp-0xc],eax）
    int i;
    i = 0;
    while (len > i) {
        p = (ushort *)(data + i);
        p2 = p;
        if (9 >= (uint)(len - i)) {
            break;
        }
        if ((len - i) < (int)p2[1]) {
            break;
        }
        dispatch((PacketHeader *)p2);
        i += p2[1];
    }
    /**
     * some information about each packet:
     * minimum packet length is 10
     * 0x00,0x01(ushort): packet type
     * 0x02,0x03(ushort): packet length
     * 0x4, 0x05(ushort): reversed1
     * 0x06, 0x07. 0x08, 0x09(uint): reversed2
     * ...: data content
     */
    return i;
}

bool CNetworkSession::dispatch(PacketHeader *packet) {
    // 原始：get_type() 调用结果直接入条件（cmp eax,1; sete al; test al,al; je 形态）
    if (get_type() == SESSION_TYPE_USER) {
        ushort packetId = packet->packetId;
        CPacketDispatcher* packetDispatcher = pSessionManager->GetPacketDispatcher();
        DispatcherFunction dispatcher = packetDispatcher->get_dispatcher(packetId);
        if (dispatcher != NULL) {
            dispatcher(pSessionManager, this, packet);
        }
    }
    return true;
}
ENUM_SESSION_TYPE CNetworkSession::get_type() {
    return type;
}
void CNetworkSession::set_type(ENUM_SESSION_TYPE t) {
    this->type = t;
}
bool CNetworkSession::Send(const char *data, int len) {
    int ret = send_packet(data, len);
    // 原始：显式 if/else 返回（cmp+jns 分支，避免 not+shr 位技巧）
    if (ret < 0) {
        return false;
    } else {
        return true;
    }
}
CNetworkSession *CNetworkSession::Connect(const char *ip, int port) {
    CNetworkSession* networkSession = new CNetworkSession();
    if (networkSession != NULL) {
        // 原始：CreateConnectionSocket == false 提前处理（xor eax,1; test/je 形态）
        if (networkSession->CreateConnectionSocket(ip, port) == false) {
            ArchiveLog("CreateConnectionSocket failed - %s", strerror(errno));
            delete networkSession;
            return NULL;
        }
        // 原始：GetSocket() 直接作为变参
        ArchiveLog("Try connect to other server sock(%d), ip(%s), port(%d)", networkSession->GetSocket(), ip, port);
        networkSession->set_type(SESSION_TYPE_SERVER);
        pSessionManager->RegisterSession(networkSession, SESSION_TYPE_SERVER);
        return networkSession;
    }
    // 原始：NULL 分支的 return NULL 位于函数末尾（je 跳到末尾的 mov eax,0）
    return NULL;
}
CNetworkSession *CNetworkSession::CreateListenSocket(int port, const char *ip) {
    int socket = 0;
    // 原始：直接检查 == false（xor eax,1 惯用法），失败分支在前
    if (CAbstractSocket<4096, 4096>::CreateListenSocket(socket, port, ip) == false) {
        ArchiveLog("CreateListenSocket failed - %s", strerror(errno));
        return NULL;
    }
    CNetworkSession *networkSession = new CNetworkSession();
    networkSession->SetSocket(socket, false);
    return networkSession;
}
void *CNetworkSession::operator new(size_t size) {
    return g_network_session_pool.malloc();
}
void CNetworkSession::operator delete(void *p, unsigned int /*size*/) {
    g_network_session_pool.free((CNetworkSession *)p);
}

CNetworkSession::~CNetworkSession() {
}

// ===== CAbstractSocket（原始位于 NetworkSession.cpp TU）=====

template<int TSizeIn, int TSizeOut>
int CAbstractSocket<TSizeIn, TSizeOut>::AcceptSocket() {
    sockaddr_in addr;
    socklen_t len;
    len = 16;
    int sock = accept(socket, (sockaddr *)&addr, &len);
    if (sock < 0) {
        // 原始：strerror(errno) 直接内联为变参
        ArchiveLog("Accept Socket[%d] Error(%s)", sock, strerror(errno));
        return -1;
    } else {
        // 原始：inet_ntoa/ntohs 直接内联为变参（ntohs 先于 inet_ntoa 求值）
        ArchiveLog("accepted other server ip(%s), port(%d), sock(%d)", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), sock);
    }
    return sock;
}

template<int MaxRecvBuf, int MaxSendBuf>
CAbstractSocket<MaxRecvBuf, MaxSendBuf>::CAbstractSocket() {
    socket = -1;
    what1 = 0;
    m_remain_recvlen = 0;
    recvBufferOffset = NULL;
    remainSendLen = 0;
    sendBufferOffset = NULL;
    Reset();
}

template<int MaxRecvBuf, int MaxSendBuf>
CAbstractSocket<MaxRecvBuf, MaxSendBuf>::~CAbstractSocket() {}

template<int MaxRecvBuf, int MaxSendBuf>
void CAbstractSocket<MaxRecvBuf, MaxSendBuf>::Reset() {
    socket = -1;
    what1 = 0;
    m_remain_recvlen = 0;
    remainSendLen = 0;
    memset(recvBuffer, 0, MaxRecvBuf);
    memset(sendBuffer, 0, MaxSendBuf);
    this->recvBufferOffset = recvBuffer;
    this->sendBufferOffset = sendBuffer;
}

template<int MaxRecvBuf, int MaxSendBuf>
void CAbstractSocket<MaxRecvBuf, MaxSendBuf>::AddTriggerSessionEventType(int sessionEventType) {
    this->triggerSessionEventType |= sessionEventType;
}

template<int MaxRecvBuf, int MaxSendBuf>
bool CAbstractSocket<MaxRecvBuf, MaxSendBuf>::ConnectPeer() {
    // 原始：先初始化 0（mov [ebp-0xc],0）再赋值
    int sock = 0;
    sock = connect(this->socket, (struct sockaddr *)&this->socket_address, 0x10);
    if (sock < 0 && errno != EINPROGRESS /*115*/ && errno != EAGAIN /*11*/ && errno != EINTR /*4*/ && errno != 0) {
        return false;
    }
    return true;
}

template<int MaxRecvBuf, int MaxSendBuf>
bool CAbstractSocket<MaxRecvBuf, MaxSendBuf>::CreateConnectionSocket(const char *ip, int port) {
    int sock = 0;  // 原始：函数入口先初始化 0（mov [ebp-0xc],0）
    this->socket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (this->socket < 0) {
        return false;
    }
    // 原始：SetNonblock == false 提前返回（xor eax,1; test/je 形态）
    if (SetNonblock(this->socket) == false) {
        return false;
    }
    memset(&this->socket_address, 0, sizeof(this->socket_address));
    this->socket_address.sin_family = AF_INET /*2*/;
    this->socket_address.sin_addr.s_addr = inet_addr(ip);
    this->socket_address.sin_port = htons((uint16_t)port);
    return ConnectPeer();
}

template<int MaxRecvBuf, int MaxSendBuf>
bool CAbstractSocket<MaxRecvBuf, MaxSendBuf>::SetNonblock(int sock) {
    int opts = fcntl(sock, F_GETFL, 0);
    int r = fcntl(sock, F_SETFL, opts | O_NONBLOCK /*0x800 , 04000*/);
    // 原始：显式 if/else 返回
    if (r < 0) {
        return false;
    } else {
        return true;
    }
}

template<int MaxRecvBuf, int MaxSendBuf>
void CAbstractSocket<MaxRecvBuf, MaxSendBuf>::disconnect() {
    shutdown(this->socket, SHUT_RD);
    close(this->socket);
    Reset();
}

template<int MaxRecvBuf, int MaxSendBuf>
int CAbstractSocket<MaxRecvBuf, MaxSendBuf>::GetSocket() {
    return this->socket;
}

template<int MaxRecvBuf, int MaxSendBuf>
int CAbstractSocket<MaxRecvBuf, MaxSendBuf>::GetTriggerSessionEventType() {
    return this->triggerSessionEventType;
}

template<int MaxRecvBuf, int MaxSendBuf>
bool CAbstractSocket<MaxRecvBuf, MaxSendBuf>::CreateListenSocket(int &outputSocket, int port, const char *ip) {
    // 原始：socketBufSize/e 在函数顶部初始化（mov [ebp-0x10],0xea60; mov [ebp-0xc],0）
    int socketBufSize = 60000;
    int e = 0;
    // 原始：socket() 结果直接写入 outputSocket，无中间变量
    outputSocket = ::socket(PF_INET, SOCK_STREAM, 0);
    if (outputSocket < 0) {
        return 0;
    } else {
        // 原始：sockaddr_in 命名字段（直接成员偏移，无地址计算/ebx 缓存）
        struct sockaddr_in sockaddr;
        memset(&sockaddr, 0, sizeof(sockaddr));
        sockaddr.sin_family = AF_INET /*2*/;
        if (ip != NULL) {
            sockaddr.sin_addr.s_addr = inet_addr(ip);
        } else {
            sockaddr.sin_addr.s_addr = htonl(0);
        }
        sockaddr.sin_port = htons((uint16_t)port);
        e = setsockopt(outputSocket, SOL_SOCKET /*1*/, SO_REUSEADDR /*2*/, &socketBufSize, 4);
        if (e < 0) {
            return 0;
        } else {
            // 原始：bind/listen 调用结果直接入条件（shr eax,0x1f + test/je）
            if (bind(outputSocket, (struct sockaddr *)&sockaddr, 0x10) < 0) {
                return 0;
            } else {
                if (listen(outputSocket, 5) < 0) {
                    return 0;
                } else {
                    // 原始：SetNonblock 结果 == false 提前返回（xor eax,1; test/je）
                    if (SetNonblock(outputSocket) == false) {
                        return 0;
                    }
                    return 1;
                }
            }
        }
    }
}

template<int MaxRecvBuf, int MaxSendBuf>
bool CAbstractSocket<MaxRecvBuf, MaxSendBuf>::SetRecvBufferOffset(int parsingLen) {
    if (parsingLen < 1) {
        return 0;
    }
    if (parsingLen < this->m_remain_recvlen) {
        if (MaxRecvBuf < parsingLen) {
            ArchiveLog("[PARSING LENGTH EXCEPTION] parsingLen > MaxRecvBuf , memmove : parsingLen = %d", parsingLen);
            return 0;
        }
        this->m_remain_recvlen = this->m_remain_recvlen - parsingLen;
        this->recvBufferOffset = this->recvBuffer + this->m_remain_recvlen;
        memmove(this->recvBuffer, this->recvBuffer + parsingLen, this->m_remain_recvlen);
    } else {
        // 原始：相等分支在前（jne 才进错误分支）
        if (this->m_remain_recvlen == parsingLen) {
            this->m_remain_recvlen = 0;
            this->recvBufferOffset = this->recvBuffer;
        } else {
            ArchiveLog("[PARSING LENGTH EXCEPTION] m_remain_recvlen(%d) > parsingLen(%d) ", this->m_remain_recvlen, parsingLen);
            return 0;
        }
    }
    return 1;
}

template<int MaxRecvBuf, int MaxSendBuf>
char *CAbstractSocket<MaxRecvBuf, MaxSendBuf>::GetRecvBuff(int getSize, int &remainRecvLen) {
    this->m_remain_recvlen += getSize;
    this->recvBufferOffset += getSize;
    remainRecvLen = this->m_remain_recvlen;
    return this->recvBuffer;
}

template<int MaxRecvBuf, int MaxSendBuf>
ssize_t CAbstractSocket<MaxRecvBuf, MaxSendBuf>::recv_packet() {
    if (this->socket < 0) {
        return 0;
    }
    errno = 0;
    size_t maxRead = MaxRecvBuf - m_remain_recvlen;
    if (maxRead == 0) {
        recvBufferOffset = recvBuffer;
        m_remain_recvlen = 0;
        maxRead = MaxRecvBuf;
    }
    ssize_t nRead = read(this->socket, this->recvBufferOffset, maxRead);
    if (nRead < 0) {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN /*two 0xb, not typo. copied from original code*/ || errno == 0) {
            return 0;
        } else {
            ArchiveLog("RECV ERROR DISCONNNECT NOW FD[%d] : %d(%s)", this->socket, errno, strerror(errno));
            return -1;
        }
    } else if (nRead == 0) {
        ArchiveLog("Recv ERROR = 0 (%d) : %s, MaxRead(%d) nRead(%d)", errno, strerror(errno), maxRead, nRead);
        return -1;
    }
    return nRead;
}

template<int MaxRecvBuf, int MaxSendBuf>
int CAbstractSocket<MaxRecvBuf, MaxSendBuf>::SetSocket(int sock, bool setNonBlock) {
    this->socket = sock;
    if (setNonBlock) {
        // 原始：直接检查 SetNonblock 结果（xor eax,1 惯用法）
        if (!SetNonblock(sock)) {
            return 0;
        }
    }
    return 1;
}

template<int MaxRecvBuf, int MaxSendBuf>
void CAbstractSocket<MaxRecvBuf, MaxSendBuf>::SetTriggerSessionEventType(int sessionEventType) {
    this->triggerSessionEventType = sessionEventType;
}

template<int MaxRecvBuf, int MaxSendBuf>
int CAbstractSocket<MaxRecvBuf, MaxSendBuf>::send_packet(const char *data, int last) {
    if (this->socket < 0) {
        return -1;
    } else if (last < 1) {
        ArchiveLog("!!!Send Packet[(%d,%d) Size(%d) Error\n", (int)*data, (int)data[1], last);
        return -1;
    } else {
        int result = 0;  // 原始：声明未使用的局部变量（mov [ebp-0xc],0）
        errno = 0;
        this->remainSendLen = this->remainSendLen + last;
        // 原始：Overflow 错误分支在前（cmp;jle 直达后续检查）
        if (this->remainSendLen > MaxSendBuf) {
            this->remainSendLen = this->remainSendLen - last;
            ArchiveLog("!!!Send Packet Overflow P_TYPE[%d] Size:Remain[%d] Last[%d]", (int)data[1], this->remainSendLen, last);
            return -1;
        }
        // 原始：偏移超出 [sendBuffer, sendBuffer+MaxSendBuf) 即错误（jb/jb 无符号比较）
        if ((this->sendBufferOffset < this->sendBuffer) || (this->sendBufferOffset >= this->sendBuffer + MaxSendBuf)) {
            this->remainSendLen = this->remainSendLen - last;
            ArchiveLog("!!!Send Packet Buffer error P_TYPE[%d] Size:Remain[%d] Last[%d]", (int)data[1], this->remainSendLen, last);
            return -1;
        } else {
            memcpy(this->sendBufferOffset, data, last);
            this->sendBufferOffset = this->sendBufferOffset + last;
            return send_packet();
        }
    }
}

template<int MaxRecvBuf, int MaxSendBuf>
int CAbstractSocket<MaxRecvBuf, MaxSendBuf>::send_packet() {
    // 原始：nSend 先初始化 0（mov [ebp-0xc],0）再赋值
    int nSend = 0;
    if (this->remainSendLen < 1) {
        return 0;
    }
    nSend = write(this->socket, this->sendBuffer, this->remainSendLen);
    if (nSend < 1) {
        if (errno == EAGAIN || errno == EINTR || errno == EAGAIN /*two 0xb, not typo. copied from original code*/ || errno == 0) {
            return 0;
        } else {
            ArchiveLog("SEND ERROR DISCONNNECT NOW FD[%d] : %d(%s)", this->socket, errno, strerror(errno));
            return -1;
        }
    } else if (nSend > 0) {
        if (nSend < this->remainSendLen) {
            this->sendBufferOffset = (this->sendBuffer + nSend);
            this->remainSendLen = this->remainSendLen - nSend;
            if (this->remainSendLen < 0) {
                return -1;
            } else {
                // 原始：> MaxSendBuf 错误分支在前（cmp;jle 直达 memmove）
                if (this->remainSendLen > MaxSendBuf) {
                    return -1;
                }
                memmove(this->sendBuffer, this->sendBufferOffset, this->remainSendLen);
                this->sendBufferOffset = (this->sendBuffer + this->remainSendLen);
            }
        } else if (this->remainSendLen < nSend) {
            ArchiveLog("offset error[Remain_Data: %d Send:%d]", this->remainSendLen, nSend);
            return -1;
        } else {
            this->sendBufferOffset = this->sendBuffer;
            this->remainSendLen = 0;
        }
    }

    return nSend;
}

// 显式实例化定义（gcc 4.4 下头文件中的声明会抑制隐式实例化，定义须在此处）
template class CAbstractSocket<4096, 4096>;
